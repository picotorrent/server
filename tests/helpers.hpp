#pragma once

#include <libtorrent/hex.hpp>
#include <libtorrent/info_hash.hpp>

namespace pt
{
    static libtorrent::info_hash_t InfoHashFromString(const std::string_view& str)
    {
        if (str.size() == 40)
        {
            lt::sha1_hash hash;
            lt::aux::from_hex({ str.data(), 40 }, hash.data());
            return lt::info_hash_t(hash);
        }

        return lt::info_hash_t();
    }
}
