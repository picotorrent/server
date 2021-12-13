#include "proxycreate.hpp"

#include <boost/log/trivial.hpp>

#include "../data/datareader.hpp"
#include "../data/models/proxy.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::Models::Proxy;
using pt::Server::RPC::ProxyCreateCommand;
using pt::Server::SessionManager;

ProxyCreateCommand::ProxyCreateCommand(sqlite3* db, std::shared_ptr<SessionManager> session)
    : m_db(db),
    m_session(session)
{
}

json ProxyCreateCommand::Execute(const json& params)
{
    if (params.is_object())
    {
        std::string name = params["name"].get<std::string>();
        int numType = params["type"].get<int>();
        std::string hostname = params["hostname"].get<std::string>();
        int port = params["port"].get<int>();
        bool proxyHostnames = params["proxy_hostnames"].get<bool>();
        bool proxyPeerConnections = params["proxy_peer_connections"].get<bool>();
        bool proxyTrackerConnections = params["proxy_tracker_connections"].get<bool>();

        std::optional<std::string> username = std::nullopt;
        if (params.contains("username")
            && !params["username"].is_null())
        {
            username = params["username"].get<std::string>();
        }

        std::optional<std::string> password = std::nullopt;
        if (params.contains("password")
            && !params["username"].is_null())
        {
            password = params["password"].get<std::string>();
        }

        if (numType < 1 || numType > 6)
        {
            return Error(1, "invalid proxy type - should be between 1-6");
        }

        auto type = static_cast<lt::settings_pack::proxy_type_t>(numType);

        auto proxy = Proxy::Create(
            m_db,
            name,
            type,
            hostname,
            port,
            username,
            password,
            proxyHostnames,
            proxyPeerConnections,
            proxyTrackerConnections);

        return Ok({ {"id", proxy->Id() } });
    }

    return Error(1, "params not an object");
}
