#include "configset.hpp"

#include <boost/log/trivial.hpp>

#include "../data/datareader.hpp"

using json = nlohmann::json;
using pt::Server::RPC::ConfigSetCommand;

ConfigSetCommand::ConfigSetCommand(sqlite3* db)
    : m_db(db)
{
}

json ConfigSetCommand::Execute(json& params)
{
    json result = json::object();

    if (params.is_object())
    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(
            m_db,
            "REPLACE INTO config (key, value) VALUES ($1, $2)",
            -1,
            &stmt,
            nullptr);

        for (auto const& [key,value] : params.items())
        {
            sqlite3_bind_text(stmt, 1, key.c_str(),          -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, value.dump().c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                BOOST_LOG_TRIVIAL(error) << "Error when updating setting: " << sqlite3_errmsg(m_db);
            }

            sqlite3_reset(stmt);
        }

        sqlite3_finalize(stmt);
    }

    return result;
}
