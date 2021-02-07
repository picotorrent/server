#include "sessionmanager.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/write_resume_data.hpp>
#include <nlohmann/json.hpp>

#include "data/models/profile.hpp"
#include "data/models/settingspack.hpp"
#include "json/torrentstatus.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::Models::Profile;
using pt::Server::SessionManager;

static std::string to_str(lt::info_hash_t hash)
{
    std::stringstream ss;
    if (hash.has_v2()) { ss << hash.v2; }
    else { ss << hash.v1; }
    return ss.str();
}

struct add_params
{
    bool muted;
};

std::shared_ptr<SessionManager> SessionManager::Load(boost::asio::io_context& io, sqlite3* db)
{
    BOOST_LOG_TRIVIAL(info) << "Reading session params";

    lt::session_params params;

    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "SELECT data FROM session_params ORDER BY timestamp DESC LIMIT 1", -1, &stmt, nullptr);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int len = sqlite3_column_bytes(stmt, 0);
            const char* buf = static_cast<const char*>(sqlite3_column_blob(stmt, 0));

            lt::error_code ec;
            lt::bdecode_node node = lt::bdecode(lt::span<const char>(buf, len), ec);

            if (ec)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to bdecode session params buffer: " << ec.message();
            }
            else
            {
                params = lt::read_session_params(node, lt::session::save_dht_state);
                BOOST_LOG_TRIVIAL(info) << "Loaded session params (" << len << " bytes)";
            }
        }

        sqlite3_finalize(stmt);
    }

    BOOST_LOG_TRIVIAL(info) << "Loading active profile";
    auto profile = Profile::GetActive(db);

    BOOST_LOG_TRIVIAL(debug) << "Loading settings from profile " << profile->Name();
    auto settings = Data::SettingsPack::GetById(db, profile->SettingsPackId());

    if (settings == nullptr)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to load settings pack with id " << profile->SettingsPackId();
    }
    else
    {
        BOOST_LOG_TRIVIAL(debug) << "Applying settings pack " << settings->Name();
        params.settings = settings->Settings();
        BOOST_LOG_TRIVIAL(info) << "Session settings loaded";
    }

    auto session = std::make_unique<lt::session>(params);
    auto sm = std::shared_ptr<SessionManager>(
        new SessionManager(io, db, std::move(session)));
    sm->LoadTorrents();

    return std::move(sm);
}

SessionManager::SessionManager(boost::asio::io_context& io, sqlite3* db, std::unique_ptr<lt::session> session)
    : m_io(io),
    m_db(db),
    m_session(std::move(session)),
    m_torrents(),
    m_timer(io)
{
    m_session->set_alert_notify(
        [this]()
        {
            boost::asio::post(
                m_io,
                std::bind(
                    &SessionManager::ReadAlerts,
                    this));
        });

    boost::system::error_code ec;
    m_timer.expires_from_now(boost::posix_time::seconds(1), ec);
    m_timer.async_wait(
        std::bind(&SessionManager::PostUpdates, this, std::placeholders::_1));
}

SessionManager::~SessionManager()
{
    BOOST_LOG_TRIVIAL(info) << "Saving session state";

    m_session->set_alert_notify([] {});
    m_timer.cancel();

    {
        std::vector<char> stateBuffer = lt::write_session_params_buf(
            m_session->session_state(),
            lt::session::save_dht_state);

        BOOST_LOG_TRIVIAL(debug) << "Saving session params (" << stateBuffer.size() << " bytes)";

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(m_db, "INSERT INTO session_params (data, timestamp) VALUES ($1, strftime('%s'));", -1, &stmt, nullptr);
        sqlite3_bind_blob(stmt, 1, stateBuffer.data(), static_cast<int>(stateBuffer.size()), SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            BOOST_LOG_TRIVIAL(warning) << "Failed to save session params data: " << sqlite3_errmsg(m_db);
        }

        sqlite3_finalize(stmt);
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
            lt::torrent_handle::flush_disk_cache
            | lt::torrent_handle::save_info_dict);

        ++num_outstanding_resume;
    }

    BOOST_LOG_TRIVIAL(info) << "Saving data for " << num_outstanding_resume << " torrent(s)";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(m_db, "UPDATE torrents SET queue_position = $1, resume_data = $2 WHERE info_hash = $3", -1, &stmt, nullptr);

    while (num_outstanding_resume > 0)
    {
        lt::alert const* tmp = m_session->wait_for_alert(lt::seconds(10));
        if (tmp == nullptr) { continue; }

        std::vector<lt::alert*> alerts;
        m_session->pop_alerts(&alerts);

        for (lt::alert* a : alerts)
        {
            lt::torrent_paused_alert* tp = lt::alert_cast<lt::torrent_paused_alert>(a);

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

            lt::save_resume_data_alert* rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --num_outstanding_resume;

            std::vector<char> buffer = lt::write_resume_data_buf(rd->params);
            std::string info_hash = to_str(rd->handle.info_hashes());

            // Store state
            sqlite3_bind_int(stmt, 1, static_cast<int>(rd->handle.status().queue_position));
            sqlite3_bind_blob(stmt, 2, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, info_hash.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to save resume data for torrent " << info_hash << ": " << sqlite3_errmsg(m_db);
            }

            sqlite3_reset(stmt);
        }
    }

    sqlite3_finalize(stmt);

    BOOST_LOG_TRIVIAL(info) << "Session state saved";
}

lt::info_hash_t SessionManager::AddTorrent(lt::add_torrent_params& params)
{
    params.userdata = lt::client_data_t(new add_params());

    m_session->async_add_torrent(params);

    return params.ti
        ? params.ti->info_hashes()
        : params.info_hashes;
}

void SessionManager::ForEachTorrent(std::function<bool(libtorrent::torrent_status const& ts)> const& iteree)
{
    for (auto const& item : m_torrents)
    {
        if (!iteree(item.second)) break;
    }
}

void SessionManager::ReloadSettings()
{
    auto profile = Profile::GetActive(m_db);
    auto settings = Data::SettingsPack::GetById(m_db, profile->SettingsPackId());

    m_session->apply_settings(settings->Settings());
}

void SessionManager::RemoveTorrent(lt::info_hash_t const& hash, bool remove_files)
{
    m_session->remove_torrent(
        m_torrents.at(hash).handle);
}

std::shared_ptr<void> SessionManager::Subscribe(std::function<void(nlohmann::json&)> sub)
{
    auto ptr = std::make_shared<std::function<void(json&)>>(std::move(sub));
    m_subscribers.push_back(ptr);
    return ptr;
}

void SessionManager::Broadcast(json& j)
{
    size_t sz = m_subscribers.size();

    m_subscribers.erase(
        std::remove_if(
            m_subscribers.begin(),
            m_subscribers.end(),
            [](auto& ptr) { return ptr.expired(); }),
        m_subscribers.end());

    size_t pruned = sz - m_subscribers.size();

    if (pruned > 0)
    {
        BOOST_LOG_TRIVIAL(debug) << "Pruned " << pruned << " subscriber";
    }

    auto tmp = m_subscribers;

    for (auto pf : tmp)
    {
        if (auto s = pf.lock())
        {
            (*s)(j);
        }
    }
}

void SessionManager::LoadTorrents()
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(m_db, "SELECT info_hash, resume_data, torrent_data FROM torrents ORDER BY queue_position ASC", -1, &stmt, nullptr);

    while (true)
    {
        int res = sqlite3_step(stmt);

        if (res != SQLITE_ROW)
        {
            if (res == SQLITE_ERROR)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to step: " << sqlite3_errmsg(m_db);
            }

            break;
        }

        const char* info_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        int resume_len = sqlite3_column_bytes(stmt, 1);
        const char* resume_buf = static_cast<const char*>(sqlite3_column_blob(stmt, 1));

        int torrent_len = sqlite3_column_bytes(stmt, 2);
        const char* torrent_buf = static_cast<const char*>(sqlite3_column_blob(stmt, 2));

        lt::error_code ec;
        lt::add_torrent_params params;
        
        if (resume_len > 0)
        {
            params = lt::read_resume_data({ resume_buf, resume_len }, ec);
        }

        if (ec)
        {
            BOOST_LOG_TRIVIAL(warning) << "Failed to read resume data for torrent: " << ec.message();
            continue;
        }

        lt::bdecode_node node = lt::bdecode({ torrent_buf, torrent_len }, ec);

        if (ec)
        {
            BOOST_LOG_TRIVIAL(warning) << "Failed to parse torrent data for torrent: " << ec.message();
            continue;
        }

        auto extra = new add_params();
        extra->muted = true;

        params.ti = std::make_shared<lt::torrent_info>(node);
        params.userdata = lt::client_data_t(extra);

        m_session->async_add_torrent(params);
    }

    sqlite3_finalize(stmt);
}

void SessionManager::ReadAlerts()
{
    std::vector<lt::alert*> alerts;
    m_session->pop_alerts(&alerts);

    for (auto const& alert : alerts)
    {
        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert* ata = lt::alert_cast<lt::add_torrent_alert>(alert);

            if (ata->error)
            {
                BOOST_LOG_TRIVIAL(error) << "Error when adding torrent: " << ata->error;
                return;
            }

            BOOST_LOG_TRIVIAL(info) << "Torrent " << to_str(ata->handle.info_hashes()) << " added";

            lt::torrent_status ts = ata->handle.status();
            m_torrents.insert({ ts.info_hashes, ts });

            add_params* extra = ata->params.userdata.get<add_params>();

            if (extra == nullptr)
            {
                BOOST_LOG_TRIVIAL(warning) << "No internal add_params for " << to_str(ts.info_hashes);
            }

            if (!extra->muted)
            {
                BOOST_LOG_TRIVIAL(info) << "Saving torrent " << to_str(ts.info_hashes) << " in database";

                lt::create_torrent ct(*ata->params.ti.get());
                std::vector<char> buffer;
                lt::bencode(std::back_inserter(buffer), ct.generate());

                // store in database
                sqlite3_stmt* stmt;
                sqlite3_prepare_v2(m_db, "INSERT INTO torrents (info_hash, queue_position, torrent_data) VALUES($1, $2, $3);", -1, &stmt, nullptr);
                sqlite3_bind_text(stmt, 1, to_str(ts.info_hashes).c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt, 2, static_cast<int>(ts.queue_position));
                sqlite3_bind_blob(stmt, 3, buffer.data(), static_cast<int>(buffer.size()), SQLITE_TRANSIENT);
                int res = sqlite3_step(stmt);

                if (res == SQLITE_DONE)
                {
                    BOOST_LOG_TRIVIAL(debug) << "Stored torrent in database (" << buffer.size() << " bytes)";
                }
                else
                {
                    BOOST_LOG_TRIVIAL(error) << "Failed to insert torrent in database: " << sqlite3_errmsg(m_db);
                }

                sqlite3_finalize(stmt);
            }

            json j;
            j["type"] = "torrent.added";
            j["torrent"] = ts;
            j["torrent"]["muted"] = extra->muted;

            Broadcast(j);

            break;
        }
        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);

            json j;
            j["type"] = "torrent.updated";
            j["torrents"] = json::object();

            for (auto const& status : sua->status)
            {
                m_torrents.at(status.info_hashes) = status;
                j["torrents"][to_str(status.info_hashes)] = status;
            }

            if (sua->status.size() > 0)
            {
                Broadcast(j);
            }

            break;
        }
        case lt::torrent_removed_alert::alert_type:
        {
            lt::torrent_removed_alert* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);
            std::string infoHash = to_str(tra->info_hashes);

            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(m_db, "DELETE FROM torrents WHERE info_hash = $1", -1, &stmt, nullptr);
            sqlite3_bind_text(stmt, 1, infoHash.c_str(), -1, SQLITE_TRANSIENT);

            switch (sqlite3_step(stmt))
            {
            case SQLITE_DONE:
                BOOST_LOG_TRIVIAL(debug) << "Torrent " << infoHash << " removed from database";
                break;
            default:
                BOOST_LOG_TRIVIAL(error) << "Error when removing torrent from database: " << sqlite3_errmsg(m_db);
                break;
            }

            sqlite3_finalize(stmt);

            m_torrents.erase(tra->info_hashes);

            json j;
            j["type"] = "torrent.removed";
            j["info_hash"] = to_str(tra->info_hashes);

            Broadcast(j);

            BOOST_LOG_TRIVIAL(info) << "Torrent " << infoHash << " removed";

            break;
        }
        }
    }
}

void SessionManager::PostUpdates(boost::system::error_code ec)
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
    m_timer.async_wait(
        std::bind(&SessionManager::PostUpdates, this, std::placeholders::_1));
}
