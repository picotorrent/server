#pragma once

#include <sstream>

#include <libtorrent/info_hash.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace libtorrent
{
    static void to_json(json& j, const libtorrent::info_hash_t& ih)
    {
        std::stringstream ss;
        if (ih.has_v2()) { ss << ih.v2; }
        else { ss << ih.v1; }

        j = ss.str();
    }
}
