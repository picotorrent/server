#include "labels.hpp"

#include <sstream>

#include "../statement.hpp"
#include "../sqliteexception.hpp"

using pika::Data::Models::Labels;

template<typename T>
static std::string ToString(const T &hash)
{
    std::stringstream ss;
    ss << hash;
    return ss.str();
}

void Labels::Delete(sqlite3 *db, const libtorrent::info_hash_t &hash)
{
    // Clear existing labels
    Statement::ForEach(
        db,
        "DELETE FROM labels WHERE addtorrentparams_id = \n"
        "   (SELECT id FROM addtorrentparams p\n"
        "       WHERE (p.info_hash_v1 = $1 AND p.info_hash_v2 IS NULL)\n"
        "       OR (p.info_hash_v1 IS NULL AND p.info_hash_v2 = $2)\n"
        "       OR (p.info_hash_v1 = $1 AND p.info_hash_v2 = $2));",
        [](const auto&) {},
        [&hash](sqlite3_stmt* stmt)
        {
            CHECK_OK(hash.has_v1()
                     ? sqlite3_bind_text(stmt, 1, ToString(hash.v1).c_str(), -1, SQLITE_TRANSIENT)
                     : sqlite3_bind_null(stmt, 1))

            CHECK_OK(hash.has_v2()
                     ? sqlite3_bind_text(stmt, 2, ToString(hash.v2).c_str(), -1, SQLITE_TRANSIENT)
                     : sqlite3_bind_null(stmt, 2))

            return SQLITE_OK;
        });
}

std::map<std::string, std::string> Labels::Get(
    sqlite3 *db,
    const libtorrent::info_hash_t &hash)
{
    std::map<std::string, std::string> labels;

    Statement::ForEach(
        db,
        "SELECT name, value FROM labels l\n"
        "JOIN addtorrentparams p ON p.id = l.addtorrentparams_id\n"
        "WHERE (p.info_hash_v1 = $1    AND p.info_hash_v2 IS NULL)\n"
        "   OR (p.info_hash_v1 IS NULL AND p.info_hash_v2 = $2)\n"
        "   OR (p.info_hash_v1 = $1    AND p.info_hash_v2 = $2);",
        [&labels](const Statement::Row &row)
        {
            labels.insert({ row.GetStdString(0), row.GetStdString(1) });
        },
        [&hash](sqlite3_stmt* stmt)
        {
            CHECK_OK(hash.has_v1()
                     ? sqlite3_bind_text(stmt, 1, ToString(hash.v1).c_str(), -1, SQLITE_TRANSIENT)
                     : sqlite3_bind_null(stmt, 1))

            CHECK_OK(hash.has_v2()
                     ? sqlite3_bind_text(stmt, 2, ToString(hash.v2).c_str(), -1, SQLITE_TRANSIENT)
                     : sqlite3_bind_null(stmt, 2))

            return SQLITE_OK;
        });

    return labels;
}

void Labels::Set(
    sqlite3 *db,
    const libtorrent::info_hash_t &hash,
    const std::map<std::string, std::string> &labels)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare(
        db,
        "INSERT INTO labels (name, value, addtorrentparams_id)\n"
        "VALUES($1, $2, (SELECT id FROM addtorrentparams p\n"
            "WHERE (p.info_hash_v1 = $3 AND p.info_hash_v2 IS NULL)\n"
            "OR (p.info_hash_v1 IS NULL AND p.info_hash_v2 = $4)\n"
            "OR (p.info_hash_v1 = $3 AND p.info_hash_v2 = $4)));",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK)
    {
        throw SQLiteException(db);
    }

    for (const auto& [key, val] : labels)
    {
        sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, val.c_str(), -1, SQLITE_TRANSIENT);

        hash.has_v1()
            ? sqlite3_bind_text(stmt, 3, ToString(hash.v1).c_str(), -1, SQLITE_TRANSIENT)
            : sqlite3_bind_null(stmt, 3);

        hash.has_v2()
            ? sqlite3_bind_text(stmt, 4, ToString(hash.v2).c_str(), -1, SQLITE_TRANSIENT)
            : sqlite3_bind_null(stmt, 4);

        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
}
