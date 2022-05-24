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
        auto str = j.get<std::string>();

        if (str.size() == 40)
        {
            lt::sha1_hash hash;
            lt::aux::from_hex({ str.c_str(), 40 }, hash.data());
            ih = lt::info_hash_t(hash);
        }
    }

    static void to_json(json& j, const libtorrent::sha1_hash& ih)
    {
        if (ih.is_all_zeros())
        {
            j = nullptr;
        }
        else
        {
            std::stringstream ss;
            ss << ih;
            j = ss.str();
        }
    }

    static void to_json(json& j, const libtorrent::sha256_hash& ih)
    {
        if (ih.is_all_zeros())
        {
            j = nullptr;
        }
        else
        {
            std::stringstream ss;
            ss << ih;
            j = ss.str();
        }
    }

    static void to_json(json& j, const libtorrent::info_hash_t& ih)
    {
        std::stringstream ss;
        if (ih.has_v2()) { ss << ih.v2; }
        else { ss << ih.v1; }

        j = ss.str();
    }
}
