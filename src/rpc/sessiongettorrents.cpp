#include "sessiongettorrents.hpp"

#include <utility>

#include "../json/infohash.hpp"
#include "../json/torrenthandle.hpp"
#include "../json/torrentstatus.hpp"
#include "../session.hpp"
#include "../torrenthandle.hpp"

using json = nlohmann::json;
using pika::RPC::SessionGetTorrentsCommand;

SessionGetTorrentsCommand::SessionGetTorrentsCommand(std::weak_ptr<ISession> session)
    : m_session(std::move(session))
{
}

json SessionGetTorrentsCommand::Execute(const json &req)
{
    auto session = m_session.lock();

    if (!session)
    {
        return Error(99, "Failed to lock session");
    }

    if (req.is_array())
    {
        json::array_t j;
        for (const auto& item : req)
        {
            lt::info_hash_t hash = item.get<lt::info_hash_t>();

            if (auto torrent = session->FindTorrent(hash))
            {
                j.push_back(torrent);
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
