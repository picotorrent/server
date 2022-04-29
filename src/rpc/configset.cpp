#include "configset.hpp"

#include "../data/models/config.hpp"

using json = nlohmann::json;
using pika::Data::Models::Config;
using pika::RPC::ConfigSetCommand;

ConfigSetCommand::ConfigSetCommand(sqlite3* db)
    : m_db(db)
{
}

json ConfigSetCommand::Execute(const json& params)
{
    if (!params.is_object())
    {
        return Error(-1, "params not an object");
    }

    for (const auto& [key,value] : params.items())
    {
        Config::Set(m_db, key, value);
    }

    return Ok();
}
