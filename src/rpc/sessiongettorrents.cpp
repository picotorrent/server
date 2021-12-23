#include "sessiongettorrents.hpp"

#include "../json/torrentstatus.hpp"
#include "../sessionmanager.hpp"

using json = nlohmann::json;
using pt::Server::RPC::SessionGetTorrentsCommand;
using pt::Server::SessionManager;

SessionGetTorrentsCommand::SessionGetTorrentsCommand(std::shared_ptr<SessionManager> sm)
    : m_session(std::move(sm))
{
}

json SessionGetTorrentsCommand::Execute(const json &)
{
    std::vector<lt::torrent_status> torrents;
    m_session->ForEachTorrent(
        [&torrents](const auto& torrent)
        {
            torrents.push_back(torrent);
            return true;
        });

    return Ok(torrents);
}
