#pragma once

#include <nlohmann/json.hpp>

namespace pt::Server::RPC
{
    class Command
    {
    public:
        virtual ~Command() {}
        virtual nlohmann::json Execute(nlohmann::json&) = 0;

        nlohmann::json Error(int code, std::string const& message, nlohmann::json data = {})
        {
            return {
                {
                    "error",
                    {
                        { "code",    code },
                        { "message", message },
                        { "data",    data }
                    }
                }
            };
        }

        nlohmann::json Ok(nlohmann::json result = {})
        {
            return {
                { "result", result }
            };
        }
    };
}
