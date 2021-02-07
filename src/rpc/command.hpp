#pragma once

#include <nlohmann/json.hpp>

namespace pt::Server::RPC
{
    class Command
    {
    public:
        virtual ~Command() {}
        virtual nlohmann::json Execute(nlohmann::json&) = 0;
    };
}
