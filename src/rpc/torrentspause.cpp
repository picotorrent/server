#include "torrentspause.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/hex.hpp>
#include <libtorrent/info_hash.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <nlohmann/json.hpp>

#include "../sessionmanager.hpp"

namespace lt = libtorrent;

using json = nlohmann::json;
using pt::Server::RPC::TorrentsPauseCommand;
using pt::Server::SessionManager;

TorrentsPauseCommand::TorrentsPauseCommand(std::shared_ptr<SessionManager> session)
    : m_session(session)
{
}

json TorrentsPauseCommand::Execute(json& j)
{
    if (j.is_array())
    {
        for (std::string const& hash : j)
        {
            lt::sha1_hash sha;
            lt::aux::from_hex(
                { hash.c_str(), 40 },
                sha.data());

            lt::torrent_status status;

            if (!m_session->FindTorrent(lt::info_hash_t(sha), status))
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

    return {};
}
