#include "torrentsremove.hpp"

#include <libtorrent/hex.hpp>
#include <libtorrent/info_hash.hpp>
#include <libtorrent/session.hpp>
#include <nlohmann/json.hpp>

#include "../sessionmanager.hpp"

namespace lt = libtorrent;

using json = nlohmann::json;
using pt::Server::RPC::TorrentsRemoveCommand;
using pt::Server::SessionManager;

TorrentsRemoveCommand::TorrentsRemoveCommand(std::shared_ptr<SessionManager> session)
    : m_session(session)
{
}

json TorrentsRemoveCommand::Execute(json& j)
{
    if (j.is_array())
    {
        for (std::string const& hash : j)
        {
            lt::sha1_hash sha;
            lt::aux::from_hex(
                { hash.c_str(), 40 },
                sha.data());

            m_session->RemoveTorrent(
                lt::info_hash_t(sha));
        }
    }

    return {};
}

