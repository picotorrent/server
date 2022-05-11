#pragma once

#include <memory>
#include <duktape.h>

namespace pika::Http
{
    class HttpListener;
}

namespace pika::Plugins::Modules
{
    class HttpModule
    {
    public:
        static void Push(duk_context* ctx, const std::shared_ptr<pika::Http::HttpListener>& http);

    private:
        static duk_ret_t Func_Get(duk_context* ctx);
    };
}
