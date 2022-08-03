#include "session.hpp"

#include <libtorrent/session.hpp>

namespace lt = libtorrent;

Napi::Object Session::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env, "Session", {
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
                    printf("%d\n", m_opts.Get("foo").ToNumber().Int32Value());
                }

                return info.Env().Undefined();
            },
            "handleAlerts",
            nullptr),
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
            auto callback = [](Napi::Env env, Napi::Function cb, const int* value)
            {
                cb.Call({ Napi::Number::New(env, 1) });
                delete value;
            };

            m_tsfn.BlockingCall(new int(1), callback);
        });
}

Session::~Session()
{
    m_tsfn.Release();
}
