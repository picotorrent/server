#include "sessionfindtorrents.hpp"

#include <libpika/bittorrent/session.hpp>

#include "../json/torrentstatus.hpp"

using json = nlohmann::json;
using pika::RPC::SessionFindTorrents;

SessionFindTorrents::SessionFindTorrents(libpika::bittorrent::ISession& session)
    : m_session(session)
{
}

json SessionFindTorrents::Execute(const json &req)
{
    if (!req.is_object())
    {
        return Error(1, "Expected an object");
    }

    const json& filters = req["filters"];

    // Set up filters. We support a few different filters, namely
    // Match torrents by info hash
    // id: [{ info_hash_v1: "" }]

    // Match torrents by name
    //

    json::array_t j;

    m_session.ForEachTorrent(
        [&j](const lt::torrent_status &status)
        {
            j.push_back(status);
        });

    return Ok(j);
}
