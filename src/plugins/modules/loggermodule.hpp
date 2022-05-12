#pragma once

#include <string>
#include <duktape.h>

namespace pika::Plugins::Modules
{
    class LoggerModule
    {
    public:
        static void Push(duk_context* ctx);
    private:
        static duk_ret_t Func_Info(duk_context* ctx);
    };
}
