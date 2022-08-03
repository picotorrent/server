#pragma once

#include <map>
#include <memory>
#include <vector>

#include <libtorrent/fwd.hpp>
#include <libtorrent/torrent_status.hpp>
#include <napi.h>

namespace libtorrent { class session; }

class Session : public Napi::ObjectWrap<Session>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    explicit Session(const Napi::CallbackInfo& info);
    ~Session() override;

private:
    Napi::Value LoadTorrent(const Napi::CallbackInfo& info);
    Napi::Value On(const Napi::CallbackInfo& info);

    void Emit(const std::string& event, const std::initializer_list<napi_value> &args);

    std::unique_ptr<libtorrent::session> m_session;
    std::map<libtorrent::info_hash_t, libtorrent::torrent_status> m_torrents;

    std::map<std::string, std::vector<Napi::FunctionReference>> m_callbacks;
    Napi::ObjectReference m_opts;
    Napi::ThreadSafeFunction m_tsfn;
};
