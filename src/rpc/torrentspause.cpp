#include "torrentspause.hpp"

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
using pt::Server::RPC::TorrentsPauseCommand;
using pt::Server::SessionManager;

TorrentsPauseCommand::TorrentsPauseCommand(std::shared_ptr<SessionManager> session)
    : m_session(std::move(session))
{
}

json TorrentsPauseCommand::Execute(const json& j)
{
    if (j.is_array())
    {
        for (lt::info_hash_t const& hash : j.get<std::vector<lt::info_hash_t>>())
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

            status.handle.pause();
        }
    }

    return Error(1, "'params' not a string or array of strings");
}
