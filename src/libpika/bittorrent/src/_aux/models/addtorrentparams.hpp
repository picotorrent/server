#pragma once

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/info_hash.hpp>

namespace libpika::data
{
    class Database;
}

namespace libpika::bittorrent::_aux::models
{
    class AddTorrentParams
    {
    public:
        static int Count(libpika::data::Database& db);
        static bool Exists(libpika::data::Database&, const libtorrent::info_hash_t &hash);
        static void ForEach(libpika::data::Database&, const std::function<void(libtorrent::add_torrent_params &)> &cb);
        static void Insert(libpika::data::Database&, const libtorrent::add_torrent_params &params, int queuePosition);
        static void Remove(libpika::data::Database&, const libtorrent::info_hash_t &hash);
        static void Update(libpika::data::Database&, const libtorrent::add_torrent_params &params, int queuePosition);
    };
}
