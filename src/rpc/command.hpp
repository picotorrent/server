#pragma once

#include <nlohmann/json.hpp>

namespace pika::RPC
{
    class Command
    {
    public:
        virtual ~Command() = default;
        virtual nlohmann::json Execute(const nlohmann::json&) = 0;

        static nlohmann::json Error(int code, std::string const& message, nlohmann::json data = {})
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
