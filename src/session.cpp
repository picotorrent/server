#include "session.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/write_resume_data.hpp>
#include <nlohmann/json.hpp>

#include "data/datareader.hpp"
#include "data/sqliteexception.hpp"
#include "data/models/listeninterface.hpp"
#include "data/models/profile.hpp"
#include "data/models/proxy.hpp"
#include "data/models/settingspack.hpp"
#include "data/statement.hpp"
#include "json/infohash.hpp"
#include "json/torrentstatus.hpp"
#include "sessioneventhandler.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::Data::Models::ListenInterface;
using pika::Data::Models::Profile;
using pika::Data::Models::Proxy;
using pika::Data::SettingsPack;
using pika::Data::Statement;
using pika::Data::SQLiteException;
using pika::Session;

static std::string to_str(lt::info_hash_t hash)
{
    std::stringstream ss;
    if (hash.has_v2()) { ss << hash.v2; }
    else { ss << hash.v1; }
    return ss.str();
}

static std::string ToString(const lt::sha1_hash &hash)
{
    std::stringstream ss;
    ss << hash;
    return ss.str();
}

static std::string ToString(const lt::sha256_hash &hash)
{
    std::stringstream ss;
    ss << hash;
    return ss.str();
}

struct Scope
{
    explicit Scope(std::string name)
        : m_name(std::move(name))
    {
        BOOST_LOG_TRIVIAL(debug) << "> " << m_name;
    }

    ~Scope()
    {
        BOOST_LOG_TRIVIAL(debug) << "< " << m_name;
    }

private:
    std::string m_name;
};

struct add_params
{
    bool muted;
};

std::shared_ptr<Session> Session::Load(boost::asio::io_context& io, sqlite3* db)
{
    BOOST_LOG_TRIVIAL(info) << "Reading session params";

    lt::session_params params;

    Statement::ForEach(
        db,
        "SELECT data FROM session_params ORDER BY timestamp DESC LIMIT 1",
        [&](Statement::Row const& row)
        {
            auto buffer = row.GetBlob(0);

            lt::error_code ec;
            lt::bdecode_node node = lt::bdecode(
                lt::span<const char>(buffer.data(), buffer.size()),
                ec);

            if (ec)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to bdecode session params buffer: " << ec.message();
            }
            else
            {
                params = lt::read_session_params(node, lt::session::save_dht_state);
                BOOST_LOG_TRIVIAL(info) << "Loaded session params (" << buffer.size() << " bytes)";
            }
        });

    BOOST_LOG_TRIVIAL(info) << "Loading session settings";

    auto session = std::shared_ptr<Session>(
        new Session(
                io,
                db,
                std::make_unique<lt::session>(params)));

    session->LoadTorrents();

    return std::move(session);
}

Session::Session(boost::asio::io_context& io, sqlite3* db, std::unique_ptr<lt::session> session)
    : m_io(io),
    m_db(db),
    m_session(std::move(session)),
    m_torrents(),
    m_timer(io),
    m_stats(lt::session_stats_metrics())
{
    m_session->set_alert_notify(
        [this]()
        {
            boost::asio::post(m_io, [this] { ReadAlerts(); });
        });

    boost::system::error_code ec;
    m_timer.expires_from_now(boost::posix_time::seconds(1), ec);
    m_timer.async_wait([this](auto && PH1) { PostUpdates(std::forward<decltype(PH1)>(PH1)); });
}

Session::~Session()
{
    m_session->set_alert_notify([] {});
    m_timer.cancel();

    std::vector<char> stateBuffer = lt::write_session_params_buf(
        m_session->session_state(),
        lt::session::save_dht_state);

    BOOST_LOG_TRIVIAL(info) << "Saving session params (" << stateBuffer.size() << " bytes)";

    try
    {
        Statement::ForEach(
            m_db,
            "INSERT INTO session_params (data, timestamp) VALUES ($1, strftime('%s'));",
            [](auto const&) {},
            [&](sqlite3_stmt* stmt)
            {
                sqlite3_bind_blob(stmt, 1, stateBuffer.data(), static_cast<int>(stateBuffer.size()), SQLITE_TRANSIENT);
            });
    }
    catch (SQLiteException&)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to save session params";
    }

    m_session->pause();

    // Save each torrents resume data
    int num_outstanding_resume = 0;
    int num_paused = 0;
    int num_failed = 0;

    auto temp = m_session->get_torrent_status([](lt::torrent_status const&) { return true; });

    for (lt::torrent_status& st : temp)
    {
        if (!st.handle.is_valid()
            || !st.has_metadata
            || !st.need_save_resume)
        {
            continue;
        }

        st.handle.save_resume_data(
            lt::torrent_handle::flush_disk_cache);

        ++num_outstanding_resume;
    }

    BOOST_LOG_TRIVIAL(info) << "Saving data for " << num_outstanding_resume << " torrent(s)";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
        m_db,
        "UPDATE torrents SET queue_position = $1, resume_data_buf = $2 WHERE info_hash_v1 = $3 AND info_hash_v2 = $4",
        -1,
        &stmt,
        nullptr);

    while (num_outstanding_resume > 0)
    {
        lt::alert const* tmp = m_session->wait_for_alert(lt::seconds(10));
        if (tmp == nullptr) { continue; }

        std::vector<lt::alert*> alerts;
        m_session->pop_alerts(&alerts);

        for (lt::alert* a : alerts)
        {
            auto* tp = lt::alert_cast<lt::torrent_paused_alert>(a);

            if (tp)
            {
                ++num_paused;
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++num_failed;
                --num_outstanding_resume;
                continue;
            }

            auto* rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --num_outstanding_resume;

            std::vector<char> buffer = lt::write_resume_data_buf(rd->params);

            // Store state
            sqlite3_bind_int(stmt, 1, static_cast<int>(rd->handle.status().queue_position));
            sqlite3_bind_blob(stmt, 2, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);

            if (rd->handle.info_hashes().has_v1())
            {
                sqlite3_bind_text(stmt, 3, ToString(rd->handle.info_hashes().v1).c_str(), -1, SQLITE_TRANSIENT);
            }
            else
            {
                sqlite3_bind_null(stmt, 3);
            }

            if (rd->handle.info_hashes().has_v2())
            {
                sqlite3_bind_text(stmt, 4, ToString(rd->handle.info_hashes().v2).c_str(), -1, SQLITE_TRANSIENT);
            }
            else
            {
                sqlite3_bind_null(stmt, 4);
            }

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                BOOST_LOG_TRIVIAL(warning)
                    << "Failed to save resume data for torrent "
                    << rd->params.name << ": "
                    << sqlite3_errmsg(m_db);
            }

            sqlite3_reset(stmt);
        }
    }

    sqlite3_finalize(stmt);

    BOOST_LOG_TRIVIAL(info) << "Session state saved";
}

class TorrentHandle : public pika::ITorrentHandle
{
public:
    explicit TorrentHandle(const lt::torrent_status& status)
        : m_status(status)
    {
    }

    virtual ~TorrentHandle() = default;

    bool IsValid() override
    {
        return m_status.handle.is_valid();
    }

    void Pause() override
    {
        m_status.handle.pause();
    }

    void Resume() override
    {
        m_status.handle.resume();
    }

private:
    lt::torrent_status m_status;
};

void Session::AddEventHandler(std::shared_ptr<ISessionEventHandler> handler)
{
    m_eventHandlers.push_back(handler);
}

std::shared_ptr<pika::ITorrentHandle> Session::FindTorrent(const lt::info_hash_t& hash)
{
    auto status = m_torrents.find(hash);

    if (status == m_torrents.end())
    {
        return nullptr;
    }

    return std::make_shared<TorrentHandle>(status->second);
}

lt::info_hash_t Session::AddTorrent(lt::add_torrent_params& params)
{
    params.userdata = lt::client_data_t(new add_params());

    m_session->async_add_torrent(params);

    return params.ti
        ? params.ti->info_hashes()
        : params.info_hashes;
}

bool Session::FindTorrent(lt::info_hash_t const& hash, lt::torrent_status& status)
{
    auto it = m_torrents.find(hash);
    if (it == m_torrents.end()) { return false; }
    status = it->second;
    return true;
}

void Session::ForEachTorrent(std::function<bool(lt::torrent_status const& ts)> const& iteree)
{
    Scope s("Session::ForEachTorrent");

    for (auto const& item : m_torrents)
    {
        if (!iteree(item.second)) break;
    }
}

void Session::RemoveTorrent(lt::info_hash_t const& hash, bool remove_files)
{
    lt::remove_flags_t flags = {};

    if (remove_files)
    {
        flags = lt::session::delete_files;
    }

    auto find = m_torrents.find(hash);

    if (find == m_torrents.end())
    {
        BOOST_LOG_TRIVIAL(warning) << "Torrent not found: " << to_str(hash);
        return;
    }

    if (!find->second.handle.is_valid())
    {
        BOOST_LOG_TRIVIAL(warning) << "Torrent handle not valid: " << to_str(hash);
        return;
    }

    m_session->remove_torrent(find->second.handle, flags);
}

void Session::LoadTorrents()
{
    int added  =  0;
    int stored = -1;

    Statement::ForEach(
        m_db,
        "SELECT COUNT(*) FROM torrents",
        [&](Statement::Row const& row)
        {
            stored = row.GetInt32(0);
        });

    BOOST_LOG_TRIVIAL(info) << "Loading " << stored << " torrents from database";

    Statement::ForEach(
        m_db,
        "SELECT resume_data_buf FROM torrents ORDER BY queue_position ASC",
        [&added, &session = m_session](Statement::Row const& row)
        {
            auto buffer = row.GetBlob(0);
            auto extra = new add_params();
            extra->muted = true;

            lt::error_code ec;
            lt::add_torrent_params params = lt::read_resume_data(buffer, ec);

            if (ec)
            {
                BOOST_LOG_TRIVIAL(error) << "Failed to read resume data: " << ec.message();
                return;
            }

            params.userdata = lt::client_data_t(extra);

            session->async_add_torrent(params);
            added++;
        });

    BOOST_LOG_TRIVIAL(info) << "Loaded " << added << " (of " << stored << ") torrent(s)";
}

void Session::ReadAlerts()
{
    std::vector<lt::alert*> alerts;
    m_session->pop_alerts(&alerts);

    for (auto const& alert : alerts)
    {
        BOOST_LOG_TRIVIAL(trace) << alert->message();

        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            auto* ata = lt::alert_cast<lt::add_torrent_alert>(alert);

            if (ata->error)
            {
                BOOST_LOG_TRIVIAL(error) << "Error when adding torrent: " << ata->error;
                return;
            }

            auto* extra = ata->params.userdata.get<add_params>();

            if (extra == nullptr)
            {
                BOOST_LOG_TRIVIAL(error) << "No internal add_params for " << to_str(ata->params.info_hashes);
                m_session->remove_torrent(ata->handle, lt::session::delete_files);
                continue;
            }

            lt::torrent_status ts = ata->handle.status();
            m_torrents.insert({ ts.info_hashes, ts });

            if (!extra->muted)
            {
                BOOST_LOG_TRIVIAL(debug) << "Saving torrent " << to_str(ts.info_hashes) << " in database";

                Statement::ForEach(
                    m_db,
                    "INSERT INTO torrents (info_hash_v1, info_hash_v2, queue_position, resume_data_buf)\n"
                    "VALUES($1, $2, $3, $4);",
                    [](auto const&) {},
                    [&](sqlite3_stmt* stmt)
                    {
                        lt::info_hash_t ih = ata->params.info_hashes;

                        if (ih.has_v1())
                        {
                            sqlite3_bind_text(stmt, 1, ToString(ih.v1).c_str(), -1, SQLITE_TRANSIENT);
                        }
                        else
                        {
                            sqlite3_bind_null(stmt, 1);
                        }

                        if (ih.has_v2())
                        {
                            sqlite3_bind_text(stmt, 2, ToString(ih.v2).c_str(), -1, SQLITE_TRANSIENT);
                        }
                        else
                        {
                            sqlite3_bind_null(stmt, 2);
                        }

                        sqlite3_bind_int(stmt, 3, static_cast<int>(ts.queue_position));

                        std::vector<char> buffer = lt::write_resume_data_buf(ata->params);
                        sqlite3_bind_blob(stmt, 4, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
                    });

                BOOST_LOG_TRIVIAL(info) << "Torrent " << to_str(ata->handle.info_hashes()) << " added";
            }

            auto th = std::make_shared<TorrentHandle>(ts);

            TriggerEvent(
                [&th](ISessionEventHandler* seh)
                {
                    // TODO: Data
                    seh->OnTorrentAdded();
                });

            break;
        }
        case lt::listen_failed_alert::alert_type:
        {
            BOOST_LOG_TRIVIAL(error) << alert->message();
            break;
        }
        case lt::metadata_received_alert::alert_type:
        {
            auto* mra = lt::alert_cast<lt::metadata_received_alert>(alert);

            BOOST_LOG_TRIVIAL(info) << "Metadata received for torrent " << mra->handle.status().name;

            mra->handle.save_resume_data(
                lt::torrent_handle::flush_disk_cache
                | lt::torrent_handle::save_info_dict
                | lt::torrent_handle::only_if_modified);

            break;
        }
        case lt::save_resume_data_alert::alert_type:
        {
            auto* srda = lt::alert_cast<lt::save_resume_data_alert>(alert);

            std::vector<char> buffer = lt::write_resume_data_buf(srda->params);

            Statement::ForEach(
                m_db,
                "UPDATE torrents SET resume_data_buf = $1 WHERE info_hash_v1 = $2 AND info_hash_v2 = $3",
                [](auto&&){},
                [&](sqlite3_stmt* stmt)
                {
                    sqlite3_bind_blob(stmt, 1, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);

                    lt::info_hash_t ih = srda->params.info_hashes;

                    if (ih.has_v1())
                    {
                        sqlite3_bind_text(stmt, 2, ToString(ih.v1).c_str(), -1, SQLITE_TRANSIENT);
                    }
                    else
                    {
                        sqlite3_bind_null(stmt, 2);
                    }

                    if (ih.has_v2())
                    {
                        sqlite3_bind_text(stmt, 3, ToString(ih.v2).c_str(), -1, SQLITE_TRANSIENT);
                    }
                    else
                    {
                        sqlite3_bind_null(stmt, 3);
                    }
                });

            BOOST_LOG_TRIVIAL(info) << "Resume data saved for " << srda->params.name;

            break;
        }
        case lt::session_stats_alert::alert_type:
        {
            auto* ssa = lt::alert_cast<lt::session_stats_alert>(alert);
            auto counters = ssa->counters();

            std::map<std::string, int64_t> data;

            for (auto const& stats : m_stats)
            {
                data.insert({ stats.name, counters[stats.value_index] });
            }

            TriggerEvent(
                [data](ISessionEventHandler* seh)
                {
                    seh->OnSessionStats(data);
                });

            break;
        }
        case lt::state_update_alert::alert_type:
        {
            auto* sua = lt::alert_cast<lt::state_update_alert>(alert);

            std::vector<std::shared_ptr<ITorrentHandle>> torrents;

            for (auto const& status : sua->status)
            {
                m_torrents.at(status.info_hashes) = status;
                torrents.push_back(std::make_shared<TorrentHandle>(status));
            }

            if (!sua->status.empty())
            {
                TriggerEvent(
                    [&torrents](ISessionEventHandler* seh)
                    {
                        // TODO: Data
                        seh->OnStateUpdate();
                    });
            }

            break;
        }
        case lt::torrent_removed_alert::alert_type:
        {
            auto* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);
            std::string hash = to_str(tra->info_hashes);

            m_torrents.erase(tra->info_hashes);

            Statement::ForEach(
                m_db,
                "DELETE FROM torrents\n"
                "WHERE (info_hash_v1 = $1 AND info_hash_v2 IS NULL)\n"
                "   OR (info_hash_v1 IS NULL AND info_hash_v2 = $2)\n"
                "   OR (info_hash_v1 = $1 AND info_hash_v2 = $2);",
                [](auto const&){},
                [&](sqlite3_stmt* stmt)
                {
                    lt::info_hash_t ih = tra->info_hashes;

                    if (ih.has_v1())
                    {
                        sqlite3_bind_text(stmt, 1, ToString(ih.v1).c_str(), -1, SQLITE_TRANSIENT);
                    }
                    else
                    {
                        sqlite3_bind_null(stmt, 1);
                    }

                    if (ih.has_v2())
                    {
                        sqlite3_bind_text(stmt, 2, ToString(ih.v2).c_str(), -1, SQLITE_TRANSIENT);
                    }
                    else
                    {
                        sqlite3_bind_null(stmt, 2);
                    }
                });

            TriggerEvent(
                [](ISessionEventHandler* seh)
                {
                    // TODO: data
                    seh->OnTorrentRemoved();
                });

            BOOST_LOG_TRIVIAL(info) << "Torrent " << tra->torrent_name() << " removed";

            break;
        }
        }
    }
}

void Session::PostUpdates(boost::system::error_code ec)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Error in timer: " << ec;
        return;
    }

    m_session->post_dht_stats();
    m_session->post_session_stats();
    m_session->post_torrent_updates();

    m_timer.expires_from_now(boost::posix_time::seconds(1), ec);
    m_timer.async_wait([this](auto && PH1) { PostUpdates(std::forward<decltype(PH1)>(PH1)); });
}

void Session::TriggerEvent(const std::function<void(ISessionEventHandler*)> &func)
{
    size_t sz = m_eventHandlers.size();

    m_eventHandlers.erase(
        std::remove_if(
            m_eventHandlers.begin(),
            m_eventHandlers.end(),
            [](auto& ptr) { return ptr.expired(); }),
        m_eventHandlers.end());

    size_t pruned = sz - m_eventHandlers.size();

    if (pruned > 0)
    {
        BOOST_LOG_TRIVIAL(debug) << "Pruned " << pruned << " event handlers";
    }

    auto tmp = m_eventHandlers;

    for (auto& pf : tmp)
    {
        if (auto s = pf.lock())
        {
            func(s.get());
        }
    }
}
