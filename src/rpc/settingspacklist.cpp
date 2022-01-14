#include "settingspacklist.hpp"

#include <boost/log/trivial.hpp>

#include "../data/statement.hpp"
#include "../sessionmanager.hpp"

using json = nlohmann::json;
using pt::Server::Data::Statement;
using pt::Server::RPC::SettingsPackList;
using pt::Server::SessionManager;

SettingsPackList::SettingsPackList(sqlite3* db)
    : m_db(db)
{
}

json SettingsPackList::Execute(const json& params)
{
    json result;

    Statement::ForEach(
        m_db,
        "SELECT id,name,description FROM settings_pack ORDER BY name ASC",
        [&](const Statement::Row& row)
        {
            result.push_back(
                {
                    { "id", row.GetInt32("id") },
                    { "name", row.GetStdString("name") },
                    { "description", row.GetStdString("description") }
                });
        });

    return Ok(result);
}
