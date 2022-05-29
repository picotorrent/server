#include "sessionremovetorrent.hpp"

#include <libtorrent/info_hash.hpp>

#include "../json/infohash.hpp"
#include "../session.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::ISession;
using pika::RPC::SessionRemoveTorrentCommand;

SessionRemoveTorrentCommand::SessionRemoveTorrentCommand(std::weak_ptr<ISession> session)
    : m_session(std::move(session))
{
}

json SessionRemoveTorrentCommand::Execute(const json& j)
{
    auto session = m_session.lock();

    if (!session)
    {
        return Error(99, "Failed to lock session");
    }

    if (j.is_array())
    {
        for (lt::info_hash_t const& hash : j.get<std::vector<lt::info_hash_t>>())
        {
            session->RemoveTorrent(hash, false);
        }
    }
    else if (j.is_object())
    {
        lt::info_hash_t hash = j["info_hash"].get<lt::info_hash_t>();
        bool removeFiles = j["remove_files"].get<bool>();
        session->RemoveTorrent(hash, removeFiles);

        return Ok({
            { "ok", true }
        });
    }

    return Error(1, "Invalid request params. Expected either an array or object.");
}
