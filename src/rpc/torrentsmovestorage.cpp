#include "torrentsmovestorage.hpp"

#include <libpika/bittorrent/session.hpp>
#include <libpika/bittorrent/torrenthandle.hpp>
#include <libtorrent/info_hash.hpp>

#include "../json/infohash.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::RPC::TorrentsMoveStorageCommand;

TorrentsMoveStorageCommand::TorrentsMoveStorageCommand(libpika::bittorrent::ISession& session)
    : m_session(session)
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

    if (auto torrent = m_session.FindTorrent(hash))
    {
        torrent->MoveStorage(savePath);
        return Ok({});
    }

    return Error(1, "Failed to find torrent based on info hash");
}
