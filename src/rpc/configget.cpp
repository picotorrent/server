#include "configget.hpp"

#include <libpika/data/database.hpp>

#include "../data/models/config.hpp"

using json = nlohmann::json;
using pika::Data::Models::Config;
using pika::RPC::ConfigGetCommand;

ConfigGetCommand::ConfigGetCommand(libpika::data::Database& db)
    : m_db(db)
{
}

json ConfigGetCommand::Execute(const json& params)
{
    if (params.is_array())
    {
        json result = json::object();

        for (const auto& val : params.get<std::vector<std::string>>())
        {
            result[val] = Config::Get(m_db, val);
        }

        return Ok(result);
    }
    else if (params.is_string())
    {
        return Ok(Config::Get(m_db, params.get<std::string>()));
    }

    return Error(-1, "no config keys specified");
}
