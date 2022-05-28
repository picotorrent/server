#include "sessiongettorrents.hpp"

#include <utility>

#include "../json/infohash.hpp"
#include "../json/torrentstatus.hpp"
#include "../session.hpp"
#include "../torrenthandle.hpp"

using json = nlohmann::json;
using pika::RPC::SessionGetTorrentsCommand;

SessionGetTorrentsCommand::SessionGetTorrentsCommand(std::shared_ptr<ISession> session)
    : m_session(std::move(session))
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

            if (auto torrent = m_session->FindTorrent(hash))
            {
                j.push_back(torrent->Status());
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
