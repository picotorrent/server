#pragma once

#include <nlohmann/json.hpp>

#include "../data/models/proxy.hpp"

using nlohmann::json;

namespace pt::Server::Data::Models
{
    static void to_json(json& j, const std::shared_ptr<Proxy>& proxy)
    {
        j = {
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
        };
    }
}
