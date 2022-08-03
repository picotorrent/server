#include <napi.h>

#include "session.hpp"

static Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Session::Init(env, exports);

    return exports;
}

NODE_API_MODULE(libtorrent, Init)
