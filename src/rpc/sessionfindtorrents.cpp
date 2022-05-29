#include "sessionfindtorrents.hpp"

#include <utility>

#include "../json/torrentstatus.hpp"
#include "../session.hpp"

using json = nlohmann::json;
using pika::RPC::SessionFindTorrents;

SessionFindTorrents::SessionFindTorrents(std::weak_ptr<ISession> session)
    : m_session(std::move(session))
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

    if (auto session = m_session.lock())
    {
        session->ForEachTorrent(
            [&j](const lt::torrent_status &status)
            {
                j.push_back(status);
            });

        return Ok(j);
    }

    return Error(99, "Failed to lock session");
}
