#pragma once

#include <map>
#include <string>

#include <libtorrent/info_hash.hpp>

namespace libpika::data
{
    class Database;
}

namespace libpika::bittorrent::_aux::models
{
    class Labels
    {
    public:
        static void Delete(
            libpika::data::Database& db,
            const libtorrent::info_hash_t &hash);

        static std::map<std::string, std::string> Get(
            libpika::data::Database& db,
            const libtorrent::info_hash_t &hash);

        static void Set(
            libpika::data::Database& db,
            const libtorrent::info_hash_t& hash,
            const std::map<std::string, std::string>& labels);
    };
}
