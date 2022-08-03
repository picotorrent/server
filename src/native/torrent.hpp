#pragma once

#include <memory>

#include <libtorrent/fwd.hpp>
#include <napi.h>

class Torrent : public Napi::ObjectWrap<Torrent>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Value New(Napi::Env env, const libtorrent::torrent_status& ts);

    explicit Torrent(const Napi::CallbackInfo& info);
    ~Torrent() override;

private:
    Napi::Value InfoHash(const Napi::CallbackInfo& info);
    Napi::Value Name(const Napi::CallbackInfo& info);

    std::unique_ptr<libtorrent::torrent_status> m_status;

    static Napi::FunctionReference* m_ctor;
};
