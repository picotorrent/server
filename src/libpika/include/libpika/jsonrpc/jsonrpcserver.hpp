#pragma once

#include <map>
#include <string>

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

namespace libpika::jsonrpc
{
    class Method;

    class JsonRpcServer
    {
    public:
        explicit JsonRpcServer(std::map<std::string, std::shared_ptr<Method>> methods);

        nlohmann::json Execute(const nlohmann::json& req);

    private:
        static bool IsValidRequestObject(const nlohmann::json& req, std::string& errorMessage);

        std::map<std::string, std::shared_ptr<Method>> m_methods;
    };
}
