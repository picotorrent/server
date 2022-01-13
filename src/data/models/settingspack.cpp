#include "settingspack.hpp"

#include <string>
#include <unordered_set>

#include <boost/log/trivial.hpp>

#include "../datareader.hpp"
#include "../sqliteexception.hpp"

namespace lt = libtorrent;
using pt::Server::Data::SettingsPack;
using pt::Server::Data::SQLiteException;

std::shared_ptr<SettingsPack> SettingsPack::Create(
    sqlite3* db,
    int sessionId,
    const libtorrent::settings_pack& settings)
{
    std::stringstream sql;
    std::stringstream params;
    sql << "INSERT INTO settings_pack (session_id,";

    for (int st = lt::settings_pack::type_bases::string_type_base;
         st < lt::settings_pack::string_types::max_string_setting_internal;
         st++)
    {
        std::string settingName = lt::name_for_setting(st);
        if (settingName.empty()) { continue; }

        sql << settingName << ",";
        params << "$" << settingName << ",";
    }

    for (int bt = lt::settings_pack::type_bases::bool_type_base;
        bt < lt::settings_pack::bool_types::max_bool_setting_internal;
        bt++)
    {
        std::string settingName = lt::name_for_setting(bt);
        if (settingName.empty()) { continue; }

        sql << settingName << ",";
        params << "$" << settingName << ",";
    }

    for (int it = lt::settings_pack::type_bases::int_type_base;
         it < lt::settings_pack::int_types::max_int_setting_internal;
         it++)
    {
        std::string settingName = lt::name_for_setting(it);
        if (settingName.empty()) { continue; }

        sql << settingName;
        params << "$" << settingName;

        if (it + 1 == lt::settings_pack::int_types::max_int_setting_internal)
        {
            break;
        }

        sql << ",";
        params << ",";
    }

    sql << ") VALUES ($session_id, " << params.str() << ");";

    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, sql.str().c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, "$session_id"), sessionId);

    if (res != SQLITE_OK)
    {
        throw SQLiteException();
    }

    for (int st = lt::settings_pack::type_bases::string_type_base;
        st < lt::settings_pack::string_types::max_string_setting_internal;
        st ++)
    {
        std::string settingName = lt::name_for_setting(st);
        if (settingName.empty()) { continue; }
        settingName = "$" + settingName;

        sqlite3_bind_text(stmt,
                          sqlite3_bind_parameter_index(stmt, settingName.c_str()),
                          settings.get_str(st).c_str(),
                          -1,
                          SQLITE_TRANSIENT);
    }

    for (int bt = lt::settings_pack::type_bases::bool_type_base;
         bt < lt::settings_pack::bool_types::max_bool_setting_internal;
         bt ++)
    {
        std::string settingName = lt::name_for_setting(bt);
        if (settingName.empty()) { continue; }
        settingName = "$" + settingName;

        sqlite3_bind_int(stmt,
                         sqlite3_bind_parameter_index(stmt, settingName.c_str()),
                         settings.get_bool(bt) ? 1 : 0);
    }

    for (int it = lt::settings_pack::type_bases::int_type_base;
         it < lt::settings_pack::int_types::max_int_setting_internal;
         it ++)
    {
        std::string settingName = lt::name_for_setting(it);
        if (settingName.empty()) { continue; }
        settingName = "$" + settingName;

        sqlite3_bind_int(stmt,
                         sqlite3_bind_parameter_index(stmt, settingName.c_str()),
                         settings.get_int(it));
    }

    res = sqlite3_step(stmt);

    if (res != SQLITE_DONE)
    {
        throw SQLiteException();
    }

    res = sqlite3_finalize(stmt);

    if (res != SQLITE_OK)
    {
        throw SQLiteException();
    }

    int id = static_cast<int>(sqlite3_last_insert_rowid(db));

    return std::shared_ptr<SettingsPack>(new SettingsPack(id, sessionId, settings));
}

std::shared_ptr<SettingsPack> SettingsPack::GetById(sqlite3* db, int id)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(
        db,
        "SELECT id,session_id,* FROM settings_pack WHERE id = $1",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to prepare SQLite statement: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    res = sqlite3_bind_int(stmt, 1, id);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to bind SQLite parameter: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    return BuildFromStatement(db, stmt);
}

std::shared_ptr<SettingsPack> SettingsPack::GetBySessionId(sqlite3* db, int sessionId)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(
        db,
        "SELECT id,session_id,* FROM settings_pack WHERE session_id = $1",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to prepare SQLite statement: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    res = sqlite3_bind_int(stmt, 1, sessionId);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to bind SQLite parameter: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    return BuildFromStatement(db, stmt);
}

std::shared_ptr<SettingsPack> SettingsPack::BuildFromStatement(sqlite3* db, sqlite3_stmt *stmt)
{
    std::shared_ptr<SettingsPack> result = nullptr;

    switch (sqlite3_step(stmt))
    {
        case SQLITE_ROW:
        {
            int id = sqlite3_column_int(stmt, 0);
            int sessionId = sqlite3_column_int(stmt, 1);

            lt::settings_pack sp;

            for (int i = 2; i < sqlite3_column_count(stmt); i++)
            {
                std::string columnName = sqlite3_column_name(stmt, i);

                int setting = lt::setting_by_name(columnName);

                if (setting == -1)
                {
                    BOOST_LOG_TRIVIAL(warning) << "Unknown setting: " << columnName;
                }

                if (setting >= lt::settings_pack::string_type_base
                    && setting < lt::settings_pack::max_string_setting_internal)
                {
                    auto res = sqlite3_column_text(stmt, i);

                    if (res != nullptr)
                    {
                        sp.set_str(
                            setting,
                            std::string(
                                reinterpret_cast<const char*>(res),
                                sqlite3_column_bytes(stmt, i)));
                    }
                }

                if (setting >= lt::settings_pack::bool_type_base
                    && setting < lt::settings_pack::max_bool_setting_internal)
                {
                    sp.set_bool(setting, sqlite3_column_int(stmt, i) == 1);
                }

                if (setting >= lt::settings_pack::int_type_base
                    && setting < lt::settings_pack::max_int_setting_internal)
                {
                    sp.set_int(setting, sqlite3_column_int(stmt, i));
                }
            }

            result.reset(new SettingsPack(id, sessionId, sp));

            break;
        }
        case SQLITE_DONE:
        {
            BOOST_LOG_TRIVIAL(warning) << "No settings_pack found";
            break;
        }
        default:
        {
            BOOST_LOG_TRIVIAL(error) << "Error when reading settings_pack: " << sqlite3_errmsg(db);
            break;
        }
    }

    sqlite3_finalize(stmt);

    return result;
}
