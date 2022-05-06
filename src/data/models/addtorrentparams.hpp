#pragma once

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/info_hash.hpp>
#include <sqlite3.h>

namespace pika::Data::Models
{
    class AddTorrentParams
    {
    public:
        static int Count(sqlite3* db);
        static bool Exists(sqlite3* db, const libtorrent::info_hash_t &hash);
        static void ForEach(sqlite3* db, const std::function<void(const libtorrent::add_torrent_params &)> &cb);
        static void Insert(sqlite3* db, const libtorrent::add_torrent_params &params, int queuePosition);
        static void Remove(sqlite3* db, const libtorrent::info_hash_t &hash);
        static void Update(sqlite3* db, const libtorrent::add_torrent_params &params);
    };
}
