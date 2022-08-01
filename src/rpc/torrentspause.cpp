#include "torrentspause.hpp"

#include <boost/log/trivial.hpp>
#include <libpika/bittorrent/session.hpp>
#include <libpika/bittorrent/torrenthandle.hpp>
#include <libtorrent/info_hash.hpp>

#include "../json/infohash.hpp"

namespace lt = libtorrent;

using json = nlohmann::json;
using pika::RPC::TorrentsPauseCommand;

TorrentsPauseCommand::TorrentsPauseCommand(libpika::bittorrent::ISession& session)
    : m_session(session)
{
}

json TorrentsPauseCommand::Execute(const json& j)
{
    auto pause = [this](const lt::info_hash_t& hash)
    {
        auto handle = m_session.FindTorrent(hash);

        if (handle == nullptr)
        {
            BOOST_LOG_TRIVIAL(warning) << "Failed to find torrent with info hash " << hash;
            return;
        }

        if (!handle->IsValid())
        {
            BOOST_LOG_TRIVIAL(warning) << "Found torrent handle which is not valid";
            return;
        }

        handle->Pause();
    };

    if (j.is_array())
    {
        for (lt::info_hash_t const& hash : j.get<std::vector<lt::info_hash_t>>())
        {
            pause(hash);
        }

        return Ok(true);
    }

    return Error(1, "Params needs to be an array of info hash arrays");
}
