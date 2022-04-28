#include "0001_initialsetup.hpp"

#include <boost/log/trivial.hpp>

using pt::Server::Data::Migrations::InitialSetup;

int InitialSetup::Migrate(sqlite3* db)
{
    BOOST_LOG_TRIVIAL(info) << "Creating torrents table";

    int res = sqlite3_exec(
        db,
        "CREATE TABLE torrents ("
            "id INTEGER PRIMARY KEY,"
            "info_hash_v1 TEXT,"
            "info_hash_v2 TEXT,"
            "queue_position INTEGER NOT NULL,"
            "resume_data_buf TEXT NOT NULL,"
            "CHECK (info_hash_v1 IS NOT NULL OR info_hash_v2 IS NOT NULL)"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    res = sqlite3_exec(
        db,
        "CREATE TABLE session_params ("
            "data      BLOB NOT NULL,"
            "timestamp INTEGER NOT NULL DEFAULT (strftime('%s'))"
        ");",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK) { return res; }

    return SQLITE_OK;
}
