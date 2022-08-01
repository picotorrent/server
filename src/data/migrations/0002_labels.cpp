#include "0002_labels.hpp"

#include <boost/log/trivial.hpp>

using pika::Data::Migrations::Labels;

int Labels::Migrate(sqlite3* db)
{
    BOOST_LOG_TRIVIAL(info) << "Creating 'labels' table";

    int res = sqlite3_exec(
        db,
        "CREATE TABLE labels ("
            "addtorrentparams_id INTEGER NOT NULL REFERENCES addtorrentparams(id),"
            "name TEXT NOT NULL,"
            "value TEXT NOT NULL,"
            "UNIQUE(addtorrentparams_id,name)"
        ");",
        nullptr,
        nullptr,
        nullptr);

    return res;
}
