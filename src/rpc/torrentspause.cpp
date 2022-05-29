#include "torrentspause.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/info_hash.hpp>
#include <nlohmann/json.hpp>

#include "../json/infohash.hpp"
#include "../session.hpp"
#include "../torrenthandle.hpp"

namespace lt = libtorrent;

using json = nlohmann::json;
using pika::RPC::TorrentsPauseCommand;
using pika::ISession;

TorrentsPauseCommand::TorrentsPauseCommand(std::weak_ptr<ISession> session)
    : m_session(std::move(session))
{
}

json TorrentsPauseCommand::Execute(const json& j)
{
    auto session = m_session.lock();

    if (!session)
    {
        return Error(99, "Failed to lock session");
    }

    auto pause = [&session](const lt::info_hash_t& hash)
    {
        auto handle = session->FindTorrent(hash);

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
