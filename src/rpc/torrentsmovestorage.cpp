#include "torrentsmovestorage.hpp"

#include <libtorrent/info_hash.hpp>

#include "../json/infohash.hpp"
#include "../session.hpp"
#include "../torrenthandle.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::ISession;
using pika::RPC::TorrentsMoveStorageCommand;

TorrentsMoveStorageCommand::TorrentsMoveStorageCommand(std::shared_ptr<ISession> session)
    : m_session(std::move(session))
{
}

json TorrentsMoveStorageCommand::Execute(const json& j)
{
    if (!j.contains("info_hash")
        || !j.contains("save_path"))
    {
        return Error(1, "Missing 'info_hash' and/or 'save_path'.");
    }

    lt::info_hash_t hash = j["info_hash"].get<lt::info_hash_t>();
    std::string savePath = j["save_path"].get<std::string>();

    if (auto torrent = m_session->FindTorrent(hash))
    {
        torrent->MoveStorage(savePath);
        return Ok({});
    }

    return Error(1, "Failed to find torrent based on info hash");
}
