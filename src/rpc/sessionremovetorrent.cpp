#include "sessionremovetorrent.hpp"

#include <libtorrent/info_hash.hpp>

#include "../json/infohash.hpp"
#include "../session.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::Session;
using pika::RPC::SessionRemoveTorrentCommand;

SessionRemoveTorrentCommand::SessionRemoveTorrentCommand(std::shared_ptr<Session> session)
    : m_session(std::move(session))
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
