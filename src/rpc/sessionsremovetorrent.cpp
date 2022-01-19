#include "sessionsremovetorrent.hpp"

#include <libtorrent/info_hash.hpp>
#include <utility>

#include "../json/infohash.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::ISessionManager;
using pt::Server::RPC::SessionsRemoveTorrentCommand;

SessionsRemoveTorrentCommand::SessionsRemoveTorrentCommand(std::shared_ptr<ISessionManager> session)
    : m_session(std::move(session))
{
}

json SessionsRemoveTorrentCommand::Execute(const json& j)
{
    if (j.is_array())
    {
        for (lt::info_hash_t const& hash : j.get<std::vector<lt::info_hash_t>>())
        {
            // m_session->RemoveTorrent(hash);
        }
    }

    return Ok();
}
