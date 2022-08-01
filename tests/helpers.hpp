#pragma once

#include <libtorrent/hex.hpp>
#include <libtorrent/info_hash.hpp>

namespace pt
{
    static libtorrent::info_hash_t InfoHashFromString(const std::string_view& str)
    {
        lt::sha1_hash hash1;
        lt::sha256_hash hash2;

        if (str.size() == 40)
        {
            lt::aux::from_hex({ str.data(), 40 }, hash1.data());
        }

        return {hash1, hash2};
    }
}
