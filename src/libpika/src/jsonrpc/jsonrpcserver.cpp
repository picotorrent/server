#include <libpika/jsonrpc/jsonrpcserver.hpp>

#include <libpika/jsonrpc/method.hpp>

using json = nlohmann::json;
using libpika::jsonrpc::JsonRpcServer;

JsonRpcServer::JsonRpcServer(std::map<std::string, std::shared_ptr<Method>> methods)
    : m_methods(std::move(methods))
{
}

nlohmann::json JsonRpcServer::Execute(const nlohmann::json &req)
{
    auto const createResponse = [](json const& input) -> nlohmann::json
    {
        json out = {
            { "jsonrpc", "2.0" }
        };

        out.merge_patch(input);

        return out;
    };

    auto const buildError = [&](int code, std::string message, const json& data = {})
    {
        json res =
            {
                { "code",    code },
                { "message", message }
            };

        if (!data.is_null())
        {
            res["data"] = data;
        }

        json err = {
            { "error", res }
        };

        return createResponse(err);
    };

    std::string errorMessage;
    if (!IsValidRequestObject(req, errorMessage))
    {
        return buildError(-32600, "Invalid request", json(errorMessage));
    }

    std::string methodName = req["method"].get<std::string>();
    auto method = m_methods.find(methodName);

    if (method == m_methods.end())
    {
        return buildError(-32601, "Method not found");
    }

    json params(nullptr);

    if (req.find("params") != req.end())
    {
        params = req["params"];
    }

    json res =
        {
            {"result", method->second->Execute(params)}
        };

    return createResponse(res);
}

bool JsonRpcServer::IsValidRequestObject(json const& req, std::string& errorMessage)
{
    if (!req.contains("jsonrpc")) { errorMessage = "Missing 'jsonrpc' key"; return false; }
    if (!req["jsonrpc"].is_string()) { errorMessage = "'jsonrpc' is not a string"; return false; }
    if (req["jsonrpc"].get<std::string>() != "2.0") { errorMessage = "Invalid JSONRPC version"; return false; }

    if (!req.contains("method")) { errorMessage = "Missing 'method' key"; return false; }
    if (!req["method"].is_string()) { errorMessage = "'method' is not a string"; return false; }

    if (req.contains("id"))
    {
        if (!req["id"].is_null()
            && !req["id"].is_number()
            && !req["id"].is_string())
        {
            errorMessage = "'id' is not a number, string or null";
            return false;
        }
    }

    return true;
}
