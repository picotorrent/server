#include "sessionaddmagnetlink.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>

#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::SessionManager;
using pt::Server::RPC::SessionAddMagnetLinkCommand;

SessionAddMagnetLinkCommand::SessionAddMagnetLinkCommand(std::shared_ptr<SessionManager> session)
    : m_session(session)
{
}

json SessionAddMagnetLinkCommand::Execute(json& j)
{
    std::string magnetUri = j["magnet_uri"].get<std::string>();

    lt::error_code ec;
    lt::add_torrent_params p = lt::parse_magnet_uri(
        magnetUri,
        ec);

    if (ec)
    {
        return Error(1, ec.message());
    }

    if (j.contains("save_path"))
    {
        p.save_path = j["save_path"];
    }

    m_session->AddTorrent(p);

    return Ok({
        { "info_hash", "asdf" }
    });
}
