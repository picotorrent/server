#pragma once

#include <duktape.h>
#include <toml++/toml.h>

namespace pika::Plugins::Modules
{
    class ConfigModule
    {
    public:
        static void Push(duk_context* ctx, toml::table tbl);
    };
}
