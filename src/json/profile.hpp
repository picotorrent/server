#pragma once

#include <nlohmann/json.hpp>

#include "../data/models/profile.hpp"

using nlohmann::json;

namespace pt::Server::Data::Models
{
    void to_json(json& j, const std::shared_ptr<Profile>& p)
    {
        j = {
            { "id",                 p->Id() },
            { "name",               p->Name() },
            { "is_active",          p->IsActive() },
            { "description",        p->Description() },
            { "proxy_id",           p->ProxyId()   ? json(p->ProxyId().value())   : json() },
            { "proxy_name",         p->ProxyName() ? json(p->ProxyName().value()) : json() },
            { "settings_pack_id",   p->SettingsPackId(), },
            { "settings_pack_name", p->SettingsPackName() }
        };
    }
}
