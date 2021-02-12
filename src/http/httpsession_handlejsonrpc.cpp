#include "httpsession.hpp"

#include <string>
#include <string_view>

#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>

#include "websocketsession.hpp"
#include "../rpc/command.hpp"

using json = nlohmann::json;
using pt::Server::Http::HttpSession;
using pt::Server::SessionManager;

bool IsValidRequestObject(json const& req, std::string& errorMessage)
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

void HttpSession::HandleJSONRPC(boost::beast::http::message<true, boost::beast::http::string_body, boost::beast::http::fields>& req)
{
    namespace http = boost::beast::http;

    auto const response = [&req](std::string const& body)
    {
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "application/json");
        res.keep_alive(req.keep_alive());
        res.body() = body;
        res.prepare_payload();
        return res;
    };

    auto const createResponse = [](json const& input)
    {
        json out = {
            { "jsonrpc", "2.0" }
        };

        out.merge_patch(input);

        return out;
    };

    auto const buildError = [&](int code, std::string message, json data = {})
    {
        json res =
        {
            { "code",    code },
            { "message", message }
        };

        if (!data.is_null())
        {
            res["data"] = data;
        };

        json err = {
            { "error", res }
        };

        return createResponse(err);
    };

    auto const executeRequest = [&](json& req)
    {
        std::string errorMessage;
        if (!IsValidRequestObject(req, errorMessage))
        {
            return buildError(-32600, "Invalid request", json(errorMessage));
        }

        std::string method = req["method"].get<std::string>();
        auto command = m_commands->find(method);

        if (command == m_commands->end())
        {
            return buildError(-32601, "Method not found");
        }

        auto& params = req["params"];
        auto const& result = command->second->Execute(params);

        return result;
    };

    json request;

    try
    {
        request = json::parse(req.body());
    }
    catch (std::exception const& ex)
    {
        return m_queue(
            response(
                buildError(-32700, "Parse error", json(ex.what())).dump()));
    }

    if (request.is_array())
    {
        json batch = json::array();

        for (auto& req : request)
        {
            batch.push_back(executeRequest(req));
        }

        return m_queue(response(batch.dump()));
    }
    else if (request.is_object())
    {
        auto result = createResponse(executeRequest(request));
        return m_queue(response(result.dump()));
    }

    m_queue(response(buildError(-32600, "Invalid request").dump()));
}
