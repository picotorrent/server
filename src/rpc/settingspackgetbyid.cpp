#include "settingspackgetbyid.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/settings_pack.hpp>

#include "../data/datareader.hpp"
#include "../data/models/settingspack.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::SettingsPack;
using pt::Server::RPC::SettingsPackGetByIdCommand;
using pt::Server::SessionManager;

SettingsPackGetByIdCommand::SettingsPackGetByIdCommand(sqlite3* db)
    : m_db(db)
{
}

json SettingsPackGetByIdCommand::Execute(const json& params)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(m_db, "SELECT sp.id,sp.description,sp.name, * FROM settings_pack sp WHERE id = $1", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, params[0].get<int>());

    json result;
    result["settings"] = json::object();

    switch (sqlite3_step(stmt))
    {
    case SQLITE_ROW:
    {
        result["id"]          = sqlite3_column_int(stmt, 0);
        result["description"] = pt_sqlite3_column_std_string(stmt, 1).value_or("");
        result["name"]        = pt_sqlite3_column_std_string(stmt, 2).value_or("");

        for (int i = 0; i < sqlite3_column_count(stmt); i++)
        {
            std::string columnName = sqlite3_column_name(stmt, i);

            if (SettingsPack::Names().find(columnName) == SettingsPack::Names().end())
            {
                continue;
            }

            int setting = lt::setting_by_name(columnName);

            if (setting == -1)
            {
                BOOST_LOG_TRIVIAL(warning) << "Unknown setting: " << columnName;
            }
            
            if (setting >= lt::settings_pack::string_type_base
                && setting < lt::settings_pack::max_string_setting_internal)
            {
                result["settings"][columnName] = pt_sqlite3_column_std_string(stmt, i).value_or("");
            }

            if (setting >= lt::settings_pack::bool_type_base
                && setting < lt::settings_pack::max_bool_setting_internal)
            {
                result["settings"][columnName] = sqlite3_column_int(stmt, i) == 1 ? true : false;
            }

            if (setting >= lt::settings_pack::int_type_base
                && setting < lt::settings_pack::max_int_setting_internal)
            {
                result["settings"][columnName] = sqlite3_column_int(stmt, i);
            }
        }

        break;
    }
    case SQLITE_ERROR:
    {
        BOOST_LOG_TRIVIAL(error) << "Error when fetching settings pack: " << sqlite3_errmsg(m_db);
        break;
    }
    }

    sqlite3_finalize(stmt);

    return Ok(result);
}
