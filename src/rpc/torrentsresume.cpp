#include "torrentsresume.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/info_hash.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <nlohmann/json.hpp>

#include "../json/infohash.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;

using json = nlohmann::json;
using pt::Server::RPC::TorrentsResumeCommand;
using pt::Server::SessionManager;

TorrentsResumeCommand::TorrentsResumeCommand(std::shared_ptr<SessionManager> session)
    : m_session(session)
{
}

json TorrentsResumeCommand::Execute(json& j)
{
    if (j.is_array())
    {
        for (lt::info_hash_t const& hash : j)
        {
            lt::torrent_status status;

            if (!m_session->FindTorrent(hash, status))
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to find torrent with info hash " << hash;
                continue;
            }

            if (!status.handle.is_valid())
            {
                BOOST_LOG_TRIVIAL(warning) << "Found torrent handle which is not valid";
                continue;
            }

            status.handle.resume();
        }
    }

    return Ok();
}
