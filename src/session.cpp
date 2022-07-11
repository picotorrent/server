#include "session.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <utility>
#include <libtorrent/write_resume_data.hpp>
#include <nlohmann/json.hpp>

#include "data/datareader.hpp"
#include "data/models/addtorrentparams.hpp"
#include "data/models/labels.hpp"
#include "data/models/sessionparams.hpp"
#include "data/sqliteexception.hpp"
#include "data/statement.hpp"
#include "torrenthandle.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::Data::Models::AddTorrentParams;
using pika::Data::Models::Labels;
using pika::Data::Models::SessionParams;
using pika::Data::Statement;
using pika::Data::SQLiteException;
using pika::Session;

struct PikaParams
{
    int current;
    int total;
};

class TorrentHandle : public pika::ITorrentHandle
{
public:
    explicit TorrentHandle(lt::torrent_status status, std::map<std::string, std::string>& labels)
        : m_status(std::move(status))
        , m_labels(labels)
    {
    }

    ~TorrentHandle() override = default;

    lt::info_hash_t InfoHash() override
    {
        return m_status.info_hashes;
    }

    bool IsValid() override
    {
        return m_status.handle.is_valid();
    }

    std::map<std::string, std::string>& Labels() override
    {
        return m_labels;
    }

    void MoveStorage(const std::string& path) override
    {
        BOOST_LOG_TRIVIAL(info) << "Moving " << m_status.name << " to " << path;
        m_status.handle.move_storage(path);
    }

    void Pause() override
    {
        m_status.handle.unset_flags(
            lt::torrent_flags::auto_managed);

        m_status.handle.pause();
    }

    void Resume() override
    {
        m_status.handle.set_flags(
            lt::torrent_flags::auto_managed);

        m_status.handle.resume();
    }

    const lt::torrent_status& Status() override
    {
        return m_status;
    }

private:
    lt::torrent_status m_status;
    std::map<std::string, std::string>& m_labels;
};

std::shared_ptr<Session> Session::Load(
    boost::asio::io_context& io,
    sqlite3* db,
    const toml::table& config)
{
    BOOST_LOG_TRIVIAL(info) << "Reading session params";

    lt::session_params params = SessionParams::GetLatest(db);
    params.settings = lt::default_settings();
    params.settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    params.settings.set_int(lt::settings_pack::alert_queue_size, 100000);

    params.settings.set_bool(lt::settings_pack::anonymous_mode, config["anonymous_mode"].value_or(false));
    params.settings.set_bool(lt::settings_pack::enable_dht,     config["enable_dht"].value_or(true));
    params.settings.set_bool(lt::settings_pack::enable_lsd,     config["enable_lsd"].value_or(true));
    params.settings.set_bool(lt::settings_pack::enable_natpmp,  config["enable_natpmp"].value_or(true));
    params.settings.set_bool(lt::settings_pack::enable_upnp,    config["enable_upnp"].value_or(true));

    if (auto* listenInterfaces = config["listen_interfaces"].as_array())
    {
        std::stringstream ss;
        for (auto&& el : *listenInterfaces)
        {
            std::string li = el.value<std::string>().value_or("");
            ss << "," << li;
        }

        params.settings.set_str(lt::settings_pack::listen_interfaces, ss.str().substr(1));
    }

    if (auto* proxy = config["proxy"].as_table())
    {
        if ((*proxy)["host"].is_string()
            && (*proxy)["port"].is_number()
            && (*proxy)["type"].is_string())
        {
            std::string host = (*proxy)["host"].value<std::string>().value();
            int port = (*proxy)["port"].value<int>().value();
            std::string type = (*proxy)["type"].value<std::string>().value();

            BOOST_LOG_TRIVIAL(info) << "Configuring proxy (" << type << " - " << host << ":" << port << ")";

            params.settings.set_str(lt::settings_pack::proxy_hostname, host);
            params.settings.set_int(lt::settings_pack::proxy_port, port);

            params.settings.set_bool(lt::settings_pack::proxy_hostnames, (*proxy)["proxy_hostnames"].value<bool>().value_or(true));
            params.settings.set_bool(lt::settings_pack::proxy_peer_connections, (*proxy)["proxy_peer_connections"].value<bool>().value_or(true));
            params.settings.set_bool(lt::settings_pack::proxy_tracker_connections, (*proxy)["proxy_tracker_connections"].value<bool>().value_or(true));

            if (type == "SOCKS5")
                params.settings.set_int(lt::settings_pack::proxy_type, lt::settings_pack::proxy_type_t::socks5);
            else
                BOOST_LOG_TRIVIAL(error) << "Unknown proxy type: " << type;
        }
        else
        {
            BOOST_LOG_TRIVIAL(error) << "Invalid proxy settings. Required fields are 'host', 'port' and 'type'.";
        }
    }

    auto session = std::make_unique<lt::session>(params);
    int torrents = AddTorrentParams::Count(db);

    BOOST_LOG_TRIVIAL(info)
        << "Loading "
        << torrents
        << " torrent(s) from database";

    int added = 0;

    AddTorrentParams::ForEach(
        db,
        [&session, &added, &torrents](lt::add_torrent_params &params)
        {
            added++;

            auto ap = new PikaParams();
            ap->current = added;
            ap->total = torrents;

            params.userdata = lt::client_data_t(ap);

            session->async_add_torrent(params);
        });

    return nullptr;
}

Session::Session(boost::asio::io_context& io, sqlite3* db, const Options& opts)
    : m_io(io)
    , m_db(db)
    , m_torrents()
    , m_timer(io)
    , m_stats(lt::session_stats_metrics())
{
    m_session = std::make_unique<lt::session>();
    m_session->set_alert_notify(
        [this]()
        {
            boost::asio::post(m_io, [this] { ReadAlerts(); });
        });

    boost::system::error_code ec;
    m_timer.expires_from_now(boost::posix_time::seconds(1), ec);

    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to set timer expiry: " << ec;
    }
    else
    {
        m_timer.async_wait(
            [this](auto &&PH1)
            {
                PostUpdates(std::forward<decltype(PH1)>(PH1));
            });
    }
}

Session::~Session()
{
    BOOST_LOG_TRIVIAL(info) << "Shutting down session";

    m_session->set_alert_notify([] {});
    m_timer.cancel();

    SessionParams::Insert(
        m_db,
        m_session->session_state());

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
            | lt::torrent_handle::save_info_dict
            | lt::torrent_handle::only_if_modified);

        ++num_outstanding_resume;
    }

    BOOST_LOG_TRIVIAL(info) << "Saving data for " << num_outstanding_resume << " torrent(s)";

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

            if (auto fail = lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++num_failed;
                --num_outstanding_resume;

                BOOST_LOG_TRIVIAL(error)
                    << "Failed to save resume data for "
                    << fail->torrent_name()
                    << ": " << fail->message();

                continue;
            }

            auto* rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --num_outstanding_resume;

            AddTorrentParams::Update(
                m_db,
                rd->params,
                static_cast<int>(rd->handle.status().queue_position));
        }
    }

    BOOST_LOG_TRIVIAL(info) << "Saving labels";

    for (const auto& [hash, labels] : m_labels)
    {
        Labels::Set(m_db, hash, labels);
    }

    BOOST_LOG_TRIVIAL(info) << "All state saved";
}

std::map<std::string, int64_t> Session::Counters()
{
    return m_metrics;
}

std::shared_ptr<pika::ITorrentHandle> Session::FindTorrent(const lt::info_hash_t& hash)
{
    auto status = m_torrents.find(hash);

    if (status == m_torrents.end())
    {
        return nullptr;
    }

    return std::make_shared<TorrentHandle>(
        status->second,
        GetOrCreateLabelsMap(status->second.info_hashes));
}

void Session::ForEachTorrent(const std::function<void(const libtorrent::torrent_status &)> &cb)
{
    for (const auto& [hash, status] : m_torrents)
    {
        cb(status);
    }
}

lt::info_hash_t Session::AddTorrent(const lt::add_torrent_params &params)
{
    m_session->async_add_torrent(params);

    return params.ti
        ? params.ti->info_hashes()
        : params.info_hashes;
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
        BOOST_LOG_TRIVIAL(warning) << "Torrent not found: " << hash;
        return;
    }

    if (!find->second.handle.is_valid())
    {
        BOOST_LOG_TRIVIAL(warning) << "Torrent handle not valid: " << hash;
        return;
    }

    m_session->remove_torrent(find->second.handle, flags);
}

std::map<std::string, std::string>& Session::GetOrCreateLabelsMap(const lt::info_hash_t &hash)
{
    if (m_labels.find(hash) == m_labels.end())
    {
        m_labels.insert({ hash, {} });
    }

    return m_labels.at(hash);
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
                continue;
            }

            lt::torrent_status ts = ata->handle.status();

            m_labels.insert({ ts.info_hashes, Labels::Get(m_db, ts.info_hashes) });
            m_torrents.insert({ ts.info_hashes, ts });

            auto* extra = ata->params.userdata.get<PikaParams>();

            if (extra == nullptr)
            {
                AddTorrentParams::Insert(m_db, ata->params, static_cast<int>(ts.queue_position));
                BOOST_LOG_TRIVIAL(info) << "Torrent " << ts.name << " added";
            }
            else
            {
                if (extra->current % 1000 == 0
                    && extra->current != extra->total)
                {
                    BOOST_LOG_TRIVIAL(info) << extra->current << " torrents (of " << extra->total << ") added";
                }
                else if (extra->current == extra->total)
                {
                    BOOST_LOG_TRIVIAL(info) << "All torrents added";
                }

                delete extra;
            }

            auto th = std::make_shared<TorrentHandle>(
                ts,
                GetOrCreateLabelsMap(ts.info_hashes));

            m_torrentAdded(th);

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
        case lt::storage_moved_alert::alert_type:
        {
            auto* sma = lt::alert_cast<lt::storage_moved_alert>(alert);

            BOOST_LOG_TRIVIAL(info)
                << "Torrent "
                << sma->torrent_name()
                << " moved to "
                << sma->storage_path();

            m_torrents.at(sma->handle.info_hashes()) = sma->handle.status();

            sma->handle.save_resume_data(
                lt::torrent_handle::flush_disk_cache
                | lt::torrent_handle::save_info_dict
                | lt::torrent_handle::only_if_modified);

            break;
        }
        case lt::storage_moved_failed_alert::alert_type:
        {
            auto* smfa = lt::alert_cast<lt::storage_moved_failed_alert>(alert);

            BOOST_LOG_TRIVIAL(error)
                << "Failed to move torrent "
                << smfa->torrent_name()
                << " to "
                << smfa->file_path()
                << ": " << smfa->message();

            break;
        }
        case lt::save_resume_data_alert::alert_type:
        {
            auto* a = lt::alert_cast<lt::save_resume_data_alert>(alert);

            AddTorrentParams::Update(
                m_db,
                a->params,
                static_cast<int>(a->handle.status().queue_position));

            BOOST_LOG_TRIVIAL(info) << "Resume data saved for " << a->params.name;

            break;
        }
        case lt::session_stats_alert::alert_type:
        {
            auto* ssa = lt::alert_cast<lt::session_stats_alert>(alert);
            auto counters = ssa->counters();

            m_metrics.clear();

            for (auto const& stats : m_stats)
            {
                m_metrics.insert({ stats.name, counters[stats.value_index] });
            }

            m_sessionStats(m_metrics);

            break;
        }
        case lt::state_update_alert::alert_type:
        {
            auto* sua = lt::alert_cast<lt::state_update_alert>(alert);

            std::vector<std::shared_ptr<ITorrentHandle>> torrents;

            for (auto const& status : sua->status)
            {
                m_torrents.at(status.info_hashes) = status;

                torrents.push_back(
                    std::make_shared<TorrentHandle>(
                        status,
                        GetOrCreateLabelsMap(status.info_hashes)));
            }

            if (!sua->status.empty())
            {
                m_stateUpdate(torrents);
            }

            break;
        }
        case lt::torrent_paused_alert::alert_type:
        {
            auto* tpa = lt::alert_cast<lt::torrent_paused_alert>(alert);
            m_torrentPaused(tpa->handle.info_hashes());
            break;
        }
        case lt::torrent_removed_alert::alert_type:
        {
            auto* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);

            m_labels.erase(tra->info_hashes);
            m_torrents.erase(tra->info_hashes);

            AddTorrentParams::Remove(m_db, tra->info_hashes);

            m_torrentRemoved(tra->info_hashes);

            BOOST_LOG_TRIVIAL(info) << "Torrent " << tra->torrent_name() << " removed";

            break;
        }
        case lt::torrent_resumed_alert::alert_type:
        {
            auto* tra = lt::alert_cast<lt::torrent_resumed_alert>(alert);
            m_torrentResumed(tra->handle.info_hashes());
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
