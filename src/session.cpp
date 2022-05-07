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
#include "data/models/sessionparams.hpp"
#include "data/sqliteexception.hpp"
#include "data/statement.hpp"
#include "scripting/scriptengine.hpp"
#include "sessioneventhandler.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::Data::Models::AddTorrentParams;
using pika::Data::Models::SessionParams;
using pika::Data::Statement;
using pika::Data::SQLiteException;
using pika::Session;

struct add_params
{
    bool muted;
};

class SettingsPackWrapper : public pika::Scripting::Wrapper
{
public:
    explicit SettingsPackWrapper(lt::settings_pack &sp)
        : m_sp(sp)
    {
    }

    duk_ret_t operator()(duk_context *ctx) override
    {
        /*duk_push_object(ctx); // target
        duk_push_pointer(ctx, this);
        duk_put_prop_string(ctx, -2, "\xff" "SettingsPackWrapper");
        duk_push_object(ctx); // handler
        duk_push_c_function(ctx, Get, 3);
        duk_put_prop_string(ctx, -2, "get");
        duk_push_c_function(ctx, Set, 3);
        duk_put_prop_string(ctx, -2, "set");
        duk_push_c_function(ctx, OwnKeys, 3);
        duk_put_prop_string(ctx, -2, "ownKeys");
        duk_push_proxy(ctx, 0);*/

        duk_push_object(ctx);
        duk_push_string(ctx, "enable_dht");
        duk_push_c_function(ctx, GetEnableDht, 0 /*nargs*/);
        duk_push_c_function(ctx, SetEnableDht, 1 /*nargs*/);
        duk_def_prop(ctx,
                     -4,
                     DUK_DEFPROP_HAVE_GETTER |
                     DUK_DEFPROP_HAVE_SETTER |
                     DUK_DEFPROP_HAVE_CONFIGURABLE |  /* clear */
                     DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE);  /* set */

        return 1;
    }

private:
    static duk_ret_t GetEnableDht(duk_context* ctx) { return 0; }
    static duk_ret_t SetEnableDht(duk_context* ctx)
    {
        return 0;
    }

    static duk_ret_t Get(duk_context* ctx)
    {
        return 0;
    }

    static duk_ret_t OwnKeys(duk_context* ctx)
    {
        duk_push_array(ctx);
        duk_push_string(ctx, "enable_dht");
        duk_put_prop_index(ctx, -2, duk_get_length(ctx, -2));
        return 1;
    }

    static duk_ret_t Set(duk_context* ctx)
    {
        // 0: target
        // 1: property
        // 2: value

        duk_get_prop_string(ctx, 0, "\xff" "SettingsPackWrapper");
        auto w = static_cast<SettingsPackWrapper*>(duk_get_pointer(ctx, -1));
        duk_pop(ctx);

        // TODO: Update properties

        return 0;
    }

    lt::settings_pack& m_sp;
};

std::shared_ptr<Session> Session::Load(
    boost::asio::io_context& io,
    sqlite3* db,
    const std::shared_ptr<pika::Scripting::IScriptEngine> &scripting)
{
    BOOST_LOG_TRIVIAL(info) << "Reading session params";

    lt::session_params params = SessionParams::GetLatest(db);
    params.settings = lt::default_settings();

    // TODO: Update settings

    SettingsPackWrapper spw(params.settings);
    scripting->Emit("session.configure", &spw);

    auto session = std::make_unique<lt::session>(params);

    BOOST_LOG_TRIVIAL(info)
        << "Loading "
        << AddTorrentParams::Count(db)
        << " from database";

    AddTorrentParams::ForEach(
        db,
        [&session](const lt::add_torrent_params &params)
        {
            session->async_add_torrent(params);
        });

    return std::shared_ptr<Session>(
        new Session(
            io,
            db,
            std::move(session)));
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
            lt::torrent_handle::flush_disk_cache);

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

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++num_failed;
                --num_outstanding_resume;
                continue;
            }

            auto* rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --num_outstanding_resume;

            AddTorrentParams::Update(m_db, rd->params);
        }
    }

    BOOST_LOG_TRIVIAL(info) << "All state saved";
}

class TorrentHandle : public pika::ITorrentHandle
{
public:
    explicit TorrentHandle(lt::torrent_status status)
        : m_status(std::move(status))
    {
    }

    ~TorrentHandle() override = default;

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
                BOOST_LOG_TRIVIAL(error) << "No internal add_params for " << ata->params.info_hashes;
                m_session->remove_torrent(ata->handle, lt::session::delete_files);
                continue;
            }

            lt::torrent_status ts = ata->handle.status();
            m_torrents.insert({ ts.info_hashes, ts });

            if (!extra->muted)
            {
                AddTorrentParams::Insert(m_db, ata->params, static_cast<int>(ts.queue_position));
                BOOST_LOG_TRIVIAL(info) << "Torrent " << ts.name << " added";
            }

            auto th = std::make_shared<TorrentHandle>(ts);

            TriggerEvent(
                [&th](ISessionEventHandler* seh)
                {
                    seh->OnTorrentAdded(th);
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
            auto* a = lt::alert_cast<lt::save_resume_data_alert>(alert);

            AddTorrentParams::Update(m_db, a->params);
            BOOST_LOG_TRIVIAL(info) << "Resume data saved for " << a->params.name;

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
                        seh->OnStateUpdate(torrents);
                    });
            }

            break;
        }
        case lt::torrent_removed_alert::alert_type:
        {
            auto* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);

            m_torrents.erase(tra->info_hashes);

            AddTorrentParams::Remove(m_db, tra->info_hashes);

            TriggerEvent(
                [&tra](ISessionEventHandler* seh)
                {
                    seh->OnTorrentRemoved(tra->info_hashes);
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
