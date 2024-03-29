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
using pt::Server::ITorrentHandleFinder;

TorrentsResumeCommand::TorrentsResumeCommand(std::shared_ptr<ITorrentHandleFinder> finder)
    : m_finder(std::move(finder))
{
}

json TorrentsResumeCommand::Execute(const json& j)
{
    auto resume = [this](const lt::info_hash_t& hash)
    {
        auto handle = m_finder->Find(hash);

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

        handle->Resume();
    };

    if (j.is_array())
    {
        for (lt::info_hash_t const& hash : j.get<std::vector<lt::info_hash_t>>())
        {
            resume(hash);
        }

        return Ok();
    }
    else if (j.is_string())
    {
        resume(j.get<lt::info_hash_t>());
        return Ok();
    }

    return Error(1, "'params' not a string or array of strings");
}
