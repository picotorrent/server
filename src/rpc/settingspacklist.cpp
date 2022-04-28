#include "settingspacklist.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/settings_pack.hpp>

#include "../data/datareader.hpp"
#include "../data/models/settingspack.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::SettingsPack;
using pt::Server::RPC::SettingsPackList;

SettingsPackList::SettingsPackList(sqlite3* db)
    : m_db(db)
{
}

json SettingsPackList::Execute(const json& params)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(m_db, "SELECT id,name,description FROM settings_pack ORDER BY name ASC", -1, &stmt, nullptr);

    json result;

    int res = SQLITE_ERROR;

    while ((res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        result.push_back(
            {
                { "id",          sqlite3_column_int(stmt, 0) },
                { "name",        pt_sqlite3_column_std_string(stmt, 1).value() },
                { "description", pt_sqlite3_column_std_string(stmt, 2).value() }
            });
    }

    sqlite3_finalize(stmt);

    return Ok(result);
}
