#include "proxygetall.hpp"

#include <boost/log/trivial.hpp>

#include "../data/datareader.hpp"
#include "../data/models/proxy.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::Models::Proxy;
using pt::Server::RPC::ProxyGetAllCommand;

ProxyGetAllCommand::ProxyGetAllCommand(sqlite3* db)
    : m_db(db)
{
}

json ProxyGetAllCommand::Execute(json& params)
{
    json result = json::array();

    for (auto const& proxy : Proxy::GetAll(m_db))
    {
        result.push_back({
            { "id",                        proxy->Id() },
            { "name",                      proxy->Name() },
            { "type",                      proxy->Type() },
            { "hostname",                  proxy->Hostname() },
            { "port",                      proxy->Port() },
            { "username",                  proxy->Username() ? json(proxy->Username().value()) : json() },
            { "password",                  proxy->Password() ? json("********") : json() },
            { "proxy_hostnames",           proxy->ProxyHostnames() },
            { "proxy_peer_connections",    proxy->ProxyPeerConnections() },
            { "proxy_tracker_connections", proxy->ProxyTrackerConnections() },
        });
    }

    return Ok(result);
}
