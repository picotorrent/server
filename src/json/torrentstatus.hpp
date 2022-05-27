#pragma once

#include <libtorrent/torrent_status.hpp>
#include <nlohmann/json.hpp>

#include "infohash.hpp"

using nlohmann::json;

namespace libtorrent
{
    static void to_json(json& j, const libtorrent::torrent_status& ts)
    {
        json ih1;
        libtorrent::to_json(ih1, ts.info_hashes.v1);

        json ih2;
        libtorrent::to_json(ih2, ts.info_hashes.v2);

        j = json {
            { "info_hash_v1", ih1 },
            { "info_hash_v2", ih2 },
            { "name",         ts.name },
            { "progress",     ts.progress },
            { "save_path",    ts.save_path },
            { "total_wanted", ts.total_wanted },
            { "state",        ts.state },
            { "dl",           ts.download_payload_rate },
            { "ul",           ts.upload_payload_rate },
            { "num_peers",    ts.num_peers },
            { "num_seeds",    ts.num_seeds },
            { "flags",        static_cast<uint64_t>(ts.flags) }
        };
    }
}
