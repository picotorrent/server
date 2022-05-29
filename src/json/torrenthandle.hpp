#pragma once

#include <libtorrent/torrent_status.hpp>
#include <nlohmann/json.hpp>

#include "infohash.hpp"
#include "../torrenthandle.hpp"

using nlohmann::json;

namespace pika
{
    static void to_json(json& j, const std::shared_ptr<pika::ITorrentHandle> &handle)
    {
        const lt::torrent_status& ts = handle->Status();

        j = json {
            { "info_hash",    ts.info_hashes },
            { "name",         ts.name },
            { "progress",     ts.progress },
            { "save_path",    ts.save_path },
            { "total_wanted", ts.total_wanted },
            { "state",        ts.state },
            { "dl",           ts.download_payload_rate },
            { "ul",           ts.upload_payload_rate },
            { "num_peers",    ts.num_peers },
            { "num_seeds",    ts.num_seeds },
            { "flags",        static_cast<uint64_t>(ts.flags) },
            { "labels",       handle->Labels() }
        };
    }
}
