#pragma once

#include <memory>

#include <napi.h>

namespace libtorrent { class session; }

class Session : public Napi::ObjectWrap<Session>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    Session(const Napi::CallbackInfo& info);
    ~Session();

private:
    std::unique_ptr<libtorrent::session> m_session;

    Napi::ObjectReference m_opts;
    Napi::ThreadSafeFunction m_tsfn;
};
