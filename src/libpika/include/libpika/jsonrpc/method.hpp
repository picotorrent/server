#pragma once

#include <nlohmann/json.hpp>

namespace libpika::jsonrpc
{
    class Method
    {
    public:
        virtual ~Method() = default;
        virtual nlohmann::json Execute(const nlohmann::json &params) = 0;

    protected:
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

        static nlohmann::json Ok(nlohmann::json result = {})
        {
            return {
                { "result", result }
            };
        }
    };
}
