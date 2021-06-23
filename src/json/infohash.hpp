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
        lt::sha1_hash sha;
        lt::aux::from_hex(
            { j.get<std::string>().c_str(), 40 },
            sha.data());

        ih = lt::info_hash_t(sha);
    }

    static void to_json(json& j, const libtorrent::info_hash_t& ih)
    {
        std::stringstream ss;
        if (ih.has_v2()) { ss << ih.v2; }
        else { ss << ih.v1; }

        j = ss.str();
    }
}
