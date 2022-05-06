#include "addtorrentparams.hpp"

#include <libtorrent/write_resume_data.hpp>

#include "../statement.hpp"

namespace lt = libtorrent;
using pika::Data::Statement;
using pika::Data::Models::AddTorrentParams;

template<typename T>
static std::string ToString(const T &hash)
{
    std::stringstream ss;
    ss << hash;
    return ss.str();
}

int AddTorrentParams::Count(sqlite3 *db)
{
    int count = -1;

    Statement::ForEach(
        db,
        "SELECT COUNT(*) FROM addtorrentparams;",
        [&count](const auto &row)
        {
            count = row.GetInt32(0);
        });

    return count;
}

bool AddTorrentParams::Exists(sqlite3 *db, const libtorrent::info_hash_t &hash)
{
    return false;
}

void AddTorrentParams::ForEach(sqlite3 *db, const std::function<void(const libtorrent::add_torrent_params &)> &cb)
{
}

void AddTorrentParams::Insert(sqlite3 *db, const libtorrent::add_torrent_params &params, int queuePosition)
{
    std::vector<char> buf = lt::write_resume_data_buf(params);

    Statement::ForEach(
        db,
        "INSERT INTO addtorrentparams\n"
        "   (info_hash_v1, info_hash_v2, queue_position, resume_data_buf)\n"
        "VALUES ($1, $2, $3, $4);",
        [](const auto &) {},
        [&hash = params.info_hashes, &buf, &queuePosition](sqlite3_stmt* stmt)
        {
            CHECK_OK(hash.has_v1()
                ? sqlite3_bind_text(stmt, 1, ToString(hash.v1).c_str(), -1, SQLITE_TRANSIENT)
                : sqlite3_bind_null(stmt, 1))

            CHECK_OK(hash.has_v2()
                ? sqlite3_bind_text(stmt, 2, ToString(hash.v2).c_str(), -1, SQLITE_TRANSIENT)
                : sqlite3_bind_null(stmt, 2))

            CHECK_OK(sqlite3_bind_int(stmt, 3, queuePosition))
            CHECK_OK(sqlite3_bind_blob(stmt, 4, buf.data(), static_cast<int>(buf.size()), SQLITE_TRANSIENT))

            return SQLITE_OK;
        });
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
