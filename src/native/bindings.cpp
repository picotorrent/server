#include <napi.h>

#include "session.hpp"
#include "torrent.hpp"

static Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    Session::Init(env, exports);
    Torrent::Init(env, exports);

    return exports;
}

NODE_API_MODULE(native, Init)
