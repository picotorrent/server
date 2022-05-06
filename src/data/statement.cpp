#include "statement.hpp"

#include <boost/log/trivial.hpp>

#include "datareader.hpp"
#include "sqliteexception.hpp"

using pika::Data::SQLiteException;
using pika::Data::Statement;

Statement::Row::Row(sqlite3_stmt* stmt)
    : m_stmt(stmt)
{
}

std::vector<char> Statement::Row::GetBlob(int col) const
{
    int len = sqlite3_column_bytes(m_stmt, col);
    const char* buf = static_cast<const char*>(sqlite3_column_blob(m_stmt, col));
    return std::vector<char>(buf, buf + len);
}

bool Statement::Row::GetBool(int col) const
{
    return sqlite3_column_int(m_stmt, col) == 1;
}

int Statement::Row::GetInt32(int col) const
{
    return sqlite3_column_int(m_stmt, col);
}

std::string Statement::Row::GetStdString(int col) const
{
    return pt_sqlite3_column_std_string(m_stmt, col).value();
}

bool Statement::Row::IsNull(int col) const
{
    return sqlite3_column_type(m_stmt, col) == SQLITE_NULL;
}

void Statement::ForEach(
    sqlite3* db,
    std::string const& sql,
    const std::function<void(Statement::Row const&)> &cb,
    const std::function<int(sqlite3_stmt*)> &bind)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to prepare statement: " << sqlite3_errmsg(db);
        throw SQLiteException(db);
    }

    if (bind)
    {
        if (bind(stmt) != SQLITE_OK)
        {
            throw SQLiteException(db);
        }
    }

    do
    {
        res = sqlite3_step(stmt);

        switch (res)
        {
        case SQLITE_DONE:
            break;
        case SQLITE_ROW: {
            Row r(stmt);
            cb(r);
            continue;
        }
        default:
            throw SQLiteException(db);
        }

        break;
    } while(true);

    if (sqlite3_finalize(stmt) != SQLITE_OK)
    {
        throw SQLiteException(db);
    }
}
