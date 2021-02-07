#pragma once

#include <libtorrent/torrent_status.hpp>
#include <nlohmann/json.hpp>

#include "infohash.hpp"

using nlohmann::json;

namespace libtorrent
{
    void to_json(json& j, const libtorrent::torrent_status& ts)
    {
        j = json {
            { "info_hash",    ts.info_hashes },
            { "name",         ts.name },
            { "progress",     ts.progress },
            { "save_path",    ts.save_path },
            { "total_wanted", ts.total_wanted },
            { "state",        ts.state },
            { "dl",           ts.download_payload_rate },
            { "ul",           ts.upload_payload_rate }
        };
    }
}
