#pragma once

#include <sstream>

#include <libtorrent/hex.hpp>
#include <libtorrent/info_hash.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace libtorrent
{
    static void from_json(const json& j, libtorrent::info_hash_t& ih)
    {
        lt::sha1_hash v1;
        lt::sha256_hash v2;

        if (j.is_array() && j.size() >= 1)
        {
            std::string v1s = j[0].get<std::string>();
            lt::aux::from_hex({ v1s.c_str() , 40 }, v1.data());
        }

        ih = lt::info_hash_t(v1, v2);
    }

    static void to_json(json& j, const libtorrent::info_hash_t& ih)
    {
        json::array_t hashes = {nullptr, nullptr};

        if (ih.has_v1())
        {
            std::stringstream ss;
            ss << ih.v1;
            hashes[0] = ss.str();
        }

        if (ih.has_v2())
        {
            std::stringstream ss;
            ss << ih.v2;
            hashes[1] = ss.str();
        }

        j = hashes;
    }
}
