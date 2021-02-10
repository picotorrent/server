#include "profileslist.hpp"

#include <boost/log/trivial.hpp>

#include "../data/datareader.hpp"

using json = nlohmann::json;
using pt::Server::RPC::ProfilesListCommand;

ProfilesListCommand::ProfilesListCommand(sqlite3* db)
    : m_db(db)
{
}

json ProfilesListCommand::Execute(json& params)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(
        m_db,
        "SELECT p.id, p.name, p.description, p.is_active, sp.id, sp.name "
        "FROM profiles p JOIN settings_pack sp ON p.settings_pack_id = sp.id "
        "ORDER BY p.name ASC",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to prepare statement: " << sqlite3_errmsg(m_db);
    }

    json result;

    while ((res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        auto name             = pt_sqlite3_column_std_string(stmt, 1);
        auto desc             = pt_sqlite3_column_std_string(stmt, 2);
        auto settingsPackName = pt_sqlite3_column_std_string(stmt, 5);

        result.push_back(
            {
                { "id",                 sqlite3_column_int(stmt, 0) },
                { "name",               name.value_or("") },
                { "description",        desc.value_or("") },
                { "is_active",          sqlite3_column_int(stmt, 3) == 1 ? true : false },
                { "settings_pack_id",   sqlite3_column_int(stmt, 4) },
                { "settings_pack_name", settingsPackName.value_or("") },
            });
    }

    sqlite3_finalize(stmt);

    return Ok(result);
}
