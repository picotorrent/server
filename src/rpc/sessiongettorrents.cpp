#include "sessiongettorrents.hpp"

#include <libpika/bittorrent/session.hpp>
#include <libpika/bittorrent/torrenthandle.hpp>

#include "../json/infohash.hpp"
#include "../json/torrenthandle.hpp"
#include "../json/torrentstatus.hpp"

using json = nlohmann::json;
using pika::RPC::SessionGetTorrentsCommand;

SessionGetTorrentsCommand::SessionGetTorrentsCommand(libpika::bittorrent::ISession& session)
    : m_session(session)
{
}

json SessionGetTorrentsCommand::Execute(const json &req)
{
    if (req.is_array())
    {
        json::array_t j;
        for (const auto& item : req)
        {
            lt::info_hash_t hash = item.get<lt::info_hash_t>();

            if (auto torrent = m_session.FindTorrent(hash))
            {
                // TODO: j.push_back(torrent);
            }
            else
            {
                j.push_back(nullptr);
            }
        }
        return Ok(j);
    }

    return Error(1, "Expected an array of info hash arrays");
}
