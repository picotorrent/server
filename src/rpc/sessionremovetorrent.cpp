#include "sessionremovetorrent.hpp"

#include <libtorrent/info_hash.hpp>

#include "../json/infohash.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::SessionManager;
using pt::Server::RPC::SessionRemoveTorrentCommand;

SessionRemoveTorrentCommand::SessionRemoveTorrentCommand(std::shared_ptr<SessionManager> session)
    : m_session(session)
{
}

json SessionRemoveTorrentCommand::Execute(const json& j)
{
    if (j.is_array())
    {
        for (lt::info_hash_t const& hash : j.get<std::vector<lt::info_hash_t>>())
        {
            m_session->RemoveTorrent(hash);
        }
    }

    return Ok();
}
