#pragma once

#include <map>
#include <string>

#include <sqlite3.h>
#include <libtorrent/info_hash.hpp>

namespace pika::Data::Models
{
    class Labels
    {
    public:
        static void Delete(
            sqlite3* db,
            const libtorrent::info_hash_t &hash);

        static std::map<std::string, std::string> Get(
            sqlite3* db,
            const libtorrent::info_hash_t &hash);

        static void Set(
            sqlite3* db,
            const libtorrent::info_hash_t& hash,
            const std::map<std::string, std::string>& labels);
    };
}
