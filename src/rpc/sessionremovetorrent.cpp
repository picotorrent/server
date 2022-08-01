#include "sessionremovetorrent.hpp"

#include <libpika/bittorrent/session.hpp>
#include <libtorrent/info_hash.hpp>

#include "../json/infohash.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::RPC::SessionRemoveTorrentCommand;

SessionRemoveTorrentCommand::SessionRemoveTorrentCommand(libpika::bittorrent::ISession& session)
    : m_session(session)
{
}

json SessionRemoveTorrentCommand::Execute(const json& j)
{
    if (j.is_array())
    {
        for (lt::info_hash_t const& hash : j.get<std::vector<lt::info_hash_t>>())
        {
            m_session.RemoveTorrent(hash, false);
        }
    }
    else if (j.is_object())
    {
        lt::info_hash_t hash = j["info_hash"].get<lt::info_hash_t>();
        bool removeFiles = j["remove_files"].get<bool>();
        m_session.RemoveTorrent(hash, removeFiles);

        return Ok({
            { "ok", true }
        });
    }

    return Error(1, "Invalid request params. Expected either an array or object.");
}
