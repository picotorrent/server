#include "addtorrentparams.hpp"

using pika::Data::Models::AddTorrentParams;

int AddTorrentParams::Count(sqlite3 *db)
{
    return -1;
}

bool AddTorrentParams::Exists(sqlite3 *db, const libtorrent::info_hash_t &hash)
{
    return false;
}

void AddTorrentParams::ForEach(sqlite3 *db, const std::function<void(const libtorrent::add_torrent_params &)> &cb)
{
}

void AddTorrentParams::Insert(sqlite3 *db, const libtorrent::add_torrent_params &params)
{
}

void AddTorrentParams::Remove(sqlite3 *db, const libtorrent::info_hash_t &hash)
{
    /*
     * Statement::ForEach(
                m_db,
                "DELETE FROM torrents\n"
                "WHERE (info_hash_v1 = $1 AND info_hash_v2 IS NULL)\n"
                "   OR (info_hash_v1 IS NULL AND info_hash_v2 = $2)\n"
                "   OR (info_hash_v1 = $1 AND info_hash_v2 = $2);",
                [](auto const&){},
                [&](sqlite3_stmt* stmt)
                {
                    lt::info_hash_t ih = tra->info_hashes;

                    if (ih.has_v1())
                    {
                        sqlite3_bind_text(stmt, 1, ToString(ih.v1).c_str(), -1, SQLITE_TRANSIENT);
                    }
                    else
                    {
                        sqlite3_bind_null(stmt, 1);
                    }

                    if (ih.has_v2())
                    {
                        sqlite3_bind_text(stmt, 2, ToString(ih.v2).c_str(), -1, SQLITE_TRANSIENT);
                    }
                    else
                    {
                        sqlite3_bind_null(stmt, 2);
                    }
                });
     */
}

void AddTorrentParams::Update(sqlite3 *db, const libtorrent::add_torrent_params &params)
{
}
