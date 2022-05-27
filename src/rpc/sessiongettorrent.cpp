#include "sessiongettorrent.hpp"

#include <utility>

#include "../json/infohash.hpp"
#include "../json/torrentstatus.hpp"
#include "../session.hpp"
#include "../torrenthandle.hpp"

using json = nlohmann::json;
using pika::RPC::SessionGetTorrentCommand;

SessionGetTorrentCommand::SessionGetTorrentCommand(std::shared_ptr<ISession> session)
    : m_session(std::move(session))
{
}

json SessionGetTorrentCommand::Execute(const json &req)
{
    lt::sha1_hash sha1 = req["info_hash_v1"].get<lt::sha1_hash>();
    lt::info_hash_t ih(sha1);

    if (auto torrent = m_session->FindTorrent(ih))
    {
        return Ok(torrent->Status());
    }

    return Error(1, "Torrent not found");
}
