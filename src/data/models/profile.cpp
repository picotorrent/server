#include "profile.hpp"

#include <boost/log/trivial.hpp>

#include "../datareader.hpp"
#include "../sqliteexception.hpp"

using pt::Server::Data::Models::Profile;
using pt::Server::Data::SQLiteException;

std::shared_ptr<Profile> Profile::GetActive(sqlite3* db)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(
        db,
        "SELECT p.id, p.name, p.description, p.is_active, sp.id, sp.name "
        "FROM profiles p JOIN settings_pack sp ON p.settings_pack_id = sp.id "
        "WHERE p.is_active = 1 LIMIT 1",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to prepare SQLite statement: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    std::shared_ptr<Profile> result = nullptr;
    res = sqlite3_step(stmt);

    switch (res)
    {
    case SQLITE_ROW:
        result.reset(new Profile());
        result->m_id               = sqlite3_column_int(stmt, 0);
        result->m_name             = pt_sqlite3_column_std_string(stmt, 1).value();
        result->m_desc             = pt_sqlite3_column_std_string(stmt, 2).value();
        result->m_isActive         = sqlite3_column_int(stmt, 3) == 1 ? true : false;
        result->m_settingsPackId   = sqlite3_column_int(stmt, 4);
        result->m_settingsPackName = pt_sqlite3_column_std_string(stmt, 5).value();
        break;

    default:
        BOOST_LOG_TRIVIAL(warning) << "Failed to find active profile: " << res;
        break;
    }

    sqlite3_finalize(stmt);

    return result;
}
