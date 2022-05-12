#include "configmodule.hpp"

#include <sstream>

using pika::Plugins::Modules::ConfigModule;

void ConfigModule::Push(duk_context *ctx, toml::table tbl)
{
    std::stringstream jsonStream;
    jsonStream << toml::json_formatter(tbl);
    std::string json = jsonStream.str();

    duk_push_string(ctx, json.c_str());
    duk_json_decode(ctx, -1);
}
