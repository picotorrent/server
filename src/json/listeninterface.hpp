#pragma once

#include <nlohmann/json.hpp>

#include "../data/models/listeninterface.hpp"

using nlohmann::json;

namespace pt::Server::Data::Models
{
    static void to_json(json& j, const std::shared_ptr<ListenInterface>& li)
    {
        j = {
            { "id",       li->Id() },
            { "host",     li->Host() },
            { "port",     li->Port() },
            { "is_local", li->IsLocal() },
            { "is_a",     li->IsOutgoing() },
            { "is_ssl",   li->IsSsl() }
        };
    }
}
