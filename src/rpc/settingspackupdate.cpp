#include "settingspackupdate.hpp"

#include <boost/log/trivial.hpp>
#include <utility>

#include "../data/transaction.hpp"
#include "../data/models/settingspack.hpp"
#include "../sessionmanager.hpp"

using json = nlohmann::json;
using pt::Server::RPC::SettingsPackUpdateCommand;
using pt::Server::ISessionManager;

SettingsPackUpdateCommand::SettingsPackUpdateCommand(
    sqlite3* db,
    std::shared_ptr<ISessionManager>  session)
    : m_db(db),
    m_session(std::move(session))
{
}

json SettingsPackUpdateCommand::Execute(const json& params)
{
    Data::Transaction tx(m_db);

    try
    {
        int id = params[0];
        bool updated_settings = false;

        if (params[1].contains("settings"))
        {
            auto settings = params[1]["settings"];

            for (auto& [key, value] : settings.items())
            {
                int setting = lt::setting_by_name(key);

                if (setting < 0)
                {
                    BOOST_LOG_TRIVIAL(warning) << "Unknown libtorrent setting: " << key;
                    continue;
                }

                std::string query = "UPDATE settings_pack SET " + key + " = $1 WHERE id = $2";

                sqlite3_stmt* stmt;
                sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr);

                if (setting >= lt::settings_pack::string_type_base
                    && setting < lt::settings_pack::max_string_setting_internal)
                {
                    sqlite3_bind_text(stmt, 1, value.get<std::string>().c_str(), -1, SQLITE_TRANSIENT);
                }
                else if (setting >= lt::settings_pack::int_type_base
                    && setting < lt::settings_pack::max_int_setting_internal)
                {
                    sqlite3_bind_int(stmt, 1, value);
                }
                else if (setting >= lt::settings_pack::bool_type_base
                    && setting < lt::settings_pack::max_bool_setting_internal)
                {
                    sqlite3_bind_int(stmt, 1, value.get<bool>() ? 1 : 0);
                }

                sqlite3_bind_int(stmt, 2, id);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }

            updated_settings = true;
        }

        tx.Commit();

        if (updated_settings)
        {
            // TODO: update related session
        }
    }
    catch (std::exception const& ex)
    {
        tx.Rollback();
        throw ex;
    }

    return Ok(true);
}
