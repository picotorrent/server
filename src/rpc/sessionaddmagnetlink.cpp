#include "sessionaddmagnetlink.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>

#include "../json/infohash.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::SessionManager;
using pt::Server::RPC::SessionAddMagnetLinkCommand;

SessionAddMagnetLinkCommand::SessionAddMagnetLinkCommand(std::shared_ptr<SessionManager> session)
    : m_session(session)
{
}

json SessionAddMagnetLinkCommand::Execute(const json& j)
{
    if (!j.contains("magnet_uri"))
    {
        return Error(1, "Missing 'magnet_uri' field");
    }

    if (!j.contains("save_path"))
    {
        return Error(1, "Missing 'save_path' field");
    }

    lt::error_code ec;
    lt::add_torrent_params p = lt::parse_magnet_uri(
        j["magnet_uri"].get<std::string>(),
        ec);

    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to parse magnet uri: " << ec.message();
        return Error(1, ec.message());
    }

    p.save_path = j["save_path"].get<std::string>();

    return Ok({
        { "info_hash", m_session->AddTorrent(p) }
    });
}
