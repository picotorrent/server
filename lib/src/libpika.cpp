#include <iostream>
#include <vector>

#include <libtorrent/session.hpp>

#define EXPORT __attribute__((visibility("default")))

extern "C"
{
    struct lt_alert
    {
        char* message;
        int type;
    };

    typedef void (*ALERT_NOTIFY_CB)();
    typedef void (*ALERT_READ_CB)(const lt_alert* alert);

    EXPORT void* lt_session_create()
    {
        libtorrent::settings_pack settings;
        settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);

        return new libtorrent::session(settings);
    }

    EXPORT void lt_session_destroy(void* ptr)
    {
        delete static_cast<libtorrent::session*>(ptr);
    }

    EXPORT void lt_session_async_add_torrent(void* ptr, void* atp)
    {
        static_cast<libtorrent::session*>(ptr)->async_add_torrent(
            *static_cast<libtorrent::add_torrent_params*>(atp));
    }

    EXPORT void lt_session_set_alert_notify(void* ptr, ALERT_NOTIFY_CB cb)
    {
        auto session = static_cast<libtorrent::session*>(ptr);
        session->set_alert_notify([&]() { cb(); });
    }

    EXPORT void lt_session_pop_alerts(void* ptr, ALERT_READ_CB cb)
    {
        auto session = static_cast<libtorrent::session*>(ptr);

        std::vector<libtorrent::alert*> alerts;
        session->pop_alerts(&alerts);

        for (auto const &alert : alerts)
        {
            std::string message = alert->message();

            lt_alert a = {};
            a.type = alert->type();
            a.message = &message[0];

            cb(&a);
        }
    }

    EXPORT bool lt_session_wait_for_alert(void* ptr)
    {
        auto session = static_cast<libtorrent::session*>(ptr);
        return session->wait_for_alert(libtorrent::seconds(1)) != nullptr;
    }

    EXPORT void* lt_atp_create()
    {
        return new libtorrent::add_torrent_params();
    }

    EXPORT void lt_atp_destroy(void* ptr)
    {
        delete static_cast<libtorrent::add_torrent_params*>(ptr);
    }

    EXPORT void lt_atp_savepath_set(void* ptr, const char* buf, int len)
    {
        auto atp = static_cast<libtorrent::add_torrent_params*>(ptr);
        atp->save_path = std::string(buf);
        printf("%s\n", atp->save_path.c_str());
    }
}
