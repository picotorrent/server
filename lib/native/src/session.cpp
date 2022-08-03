#include "session.hpp"

#include <libtorrent/alert_types.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/read_resume_data.hpp>

#include "torrent.hpp"

namespace lt = libtorrent;

Napi::Object Session::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env, "Session", {
        InstanceMethod<&Session::LoadTorrent>("loadTorrent", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&Session::On>("on", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    auto* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("Session", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

Session::Session(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Session>(info)
{
    m_opts = Napi::ObjectReference::New(info[0].As<Napi::Object>());
    m_tsfn = Napi::ThreadSafeFunction::New(
        info.Env(),
        Napi::Function::New(
            info.Env(),
            [&](const Napi::CallbackInfo& info)
            {
                std::vector<lt::alert*> alerts;
                m_session->pop_alerts(&alerts);

                for (const auto alert : alerts)
                {
                    switch (alert->type())
                    {
                        case lt::add_torrent_alert::alert_type:
                        {
                            auto *ata = lt::alert_cast<lt::add_torrent_alert>(alert);

                            if (ata->error)
                            {
                                Emit("torrents:added", {
                                    Napi::String::New(info.Env(), ata->error.message()),
                                    info.Env().Undefined()
                                });

                                continue;
                            }

                            lt::torrent_status ts = ata->handle.status();
                            m_torrents.insert({ts.info_hashes, ts});

                            Emit("torrents:added", {
                                info.Env().Undefined(),
                                Torrent::New(info.Env(), ts)
                            });

                            break;
                        }
                        case lt::listen_failed_alert::alert_type:
                        {
                            // TODO(events) emit error event
                            break;
                        }
                        case lt::metadata_received_alert::alert_type:
                        {
                            auto *mra = lt::alert_cast<lt::metadata_received_alert>(alert);

                            // TODO(events) emit event and let JS code call save resume data

                            mra->handle.save_resume_data(
                                    lt::torrent_handle::flush_disk_cache
                                    | lt::torrent_handle::save_info_dict
                                    | lt::torrent_handle::only_if_modified);

                            break;
                        }
                        case lt::storage_moved_alert::alert_type:
                        {
                            auto *sma = lt::alert_cast<lt::storage_moved_alert>(alert);

                            m_torrents.at(sma->handle.info_hashes()) = sma->handle.status();

                            // TODO(events) emit event and let JS code call save resume data

                            sma->handle.save_resume_data(
                                    lt::torrent_handle::flush_disk_cache
                                    | lt::torrent_handle::save_info_dict
                                    | lt::torrent_handle::only_if_modified);

                            break;
                        }
                        case lt::storage_moved_failed_alert::alert_type:
                        {
                            auto *smfa = lt::alert_cast<lt::storage_moved_failed_alert>(alert);

                            // TODO(events) emit event

                            break;
                        }
                        case lt::save_resume_data_alert::alert_type:
                        {
                            auto *a = lt::alert_cast<lt::save_resume_data_alert>(alert);

                            /*AddTorrentParams::Update(
                                    m_db,
                                    a->params,
                                    static_cast<int>(a->handle.status().queue_position));

                            BOOST_LOG_TRIVIAL(info) << "Resume data saved for " << a->params.name;*/
                            // TODO(events)

                            break;
                        }
                        case lt::session_stats_alert::alert_type:
                        {
                            auto *ssa = lt::alert_cast<lt::session_stats_alert>(alert);
                            auto counters = ssa->counters();

                            /*m_metrics.clear();

                            for (auto const &stats: m_stats) {
                                m_metrics.insert({stats.name, counters[stats.value_index]});
                            }

                            m_sessionStats(m_metrics);*/

                            // TODO(events)

                            break;
                        }
                        case lt::state_update_alert::alert_type:
                        {
                            auto *sua = lt::alert_cast<lt::state_update_alert>(alert);

                            /*std::vector<std::shared_ptr<ITorrentHandle>> torrents;

                            for (auto const &status: sua->status) {
                                m_torrents.at(status.info_hashes) = status;

                                torrents.push_back(
                                        std::make_shared<TorrentHandle>(
                                                status,
                                                GetOrCreateLabelsMap(status.info_hashes)));
                            }

                            if (!sua->status.empty()) {
                                m_stateUpdate(torrents);
                            }*/

                            // TODO(events)

                            break;
                        }
                        case lt::torrent_paused_alert::alert_type:
                        {
                            auto *tpa = lt::alert_cast<lt::torrent_paused_alert>(alert);
                            // TODO(events)
                            break;
                        }
                        case lt::torrent_removed_alert::alert_type:
                        {
                            auto *tra = lt::alert_cast<lt::torrent_removed_alert>(alert);

                            m_torrents.erase(tra->info_hashes);

                            /*AddTorrentParams::Remove(m_db, tra->info_hashes);

                            m_torrentRemoved(tra->info_hashes);

                            BOOST_LOG_TRIVIAL(info) << "Torrent " << tra->torrent_name() << " removed";*/

                            // TODO(events)

                            break;
                        }
                        case lt::torrent_resumed_alert::alert_type:
                        {
                            auto *tra = lt::alert_cast<lt::torrent_resumed_alert>(alert);
                            // TODO(events)
                            break;
                        }
                    }
                }

                return info.Env().Undefined();
            },
            "handleAlerts"),
        "Session",
        0,
        1,
        [](Napi::Env env) {});

    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);

    m_session = std::make_unique<lt::session>(settings);
    m_session->set_alert_notify(
        [&]()
        {
            m_tsfn.BlockingCall(
                [](Napi::Env env, Napi::Function cb)
                {
                    cb.Call({});
                });
        });
}

Session::~Session()
{
    m_session->set_alert_notify([](){});
    m_tsfn.Release();
}

Napi::Value Session::LoadTorrent(const Napi::CallbackInfo& info)
{
    auto buf = info[0].As<Napi::Uint8Array>();
    auto* data = reinterpret_cast<const char*>(buf.Data());

    lt::error_code ec;
    lt::add_torrent_params p = lt::read_resume_data(
        lt::span(data, static_cast<long>(buf.ByteLength())),
        ec);

    if (ec)
    {
        Napi::TypeError::New(info.Env(), ec.message());
        return info.Env().Undefined();
    }

    m_session->async_add_torrent(p);

    return info.Env().Undefined();
}

Napi::Value Session::On(const Napi::CallbackInfo& info)
{
    auto event = info[0].ToString();

    if (m_callbacks.find(event) == m_callbacks.end())
    {
        m_callbacks.insert({ event, std::vector<Napi::FunctionReference>() });
    }

    m_callbacks.at(event).push_back(
        Napi::Persistent(info[1].As<Napi::Function>()));

    return info.Env().Undefined();
}

void Session::Emit(const std::string &event, const std::initializer_list<napi_value> &args)
{
    if (m_callbacks.find(event) != m_callbacks.end())
    {
        for (auto const& ref : m_callbacks.at(event))
        {
            ref.Call(args);
        }
    }
}
