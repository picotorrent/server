#include "jsonrpchandler.hpp"

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

#include "../../rpc/configget.hpp"
#include "../../rpc/configset.hpp"
#include "../../rpc/sessionaddtorrent.hpp"
#include "../../rpc/sessiongettorrents.hpp"
#include "../../rpc/sessionfindtorrents.hpp"
#include "../../rpc/sessionremovetorrent.hpp"
#include "../../rpc/torrentsmovestorage.hpp"
#include "../../rpc/torrentspause.hpp"
#include "../../rpc/torrentsresume.hpp"

using json = nlohmann::json;
using pika::Http::Handlers::JsonRpcHandler;

static bool IsValidRequestObject(json const& req, std::string& errorMessage)
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

JsonRpcHandler::JsonRpcHandler(sqlite3* db, const std::shared_ptr<pika::Session> &session)
{
    m_commands.insert({ "config.get",            std::make_shared<pika::RPC::ConfigGetCommand>(db) });
    m_commands.insert({ "config.set",            std::make_shared<pika::RPC::ConfigSetCommand>(db) });
    m_commands.insert({ "session.addTorrent",    std::make_shared<pika::RPC::SessionAddTorrentCommand>(session) });
    m_commands.insert({ "session.getTorrents",   std::make_shared<pika::RPC::SessionGetTorrentsCommand>(session) });
    m_commands.insert({ "session.findTorrents",  std::make_shared<pika::RPC::SessionFindTorrents>(session) });
    m_commands.insert({ "session.removeTorrent", std::make_shared<pika::RPC::SessionRemoveTorrentCommand>(session) });
    m_commands.insert({ "torrents.moveStorage",  std::make_shared<pika::RPC::TorrentsMoveStorageCommand>(session) });
    m_commands.insert({ "torrents.pause",        std::make_shared<pika::RPC::TorrentsPauseCommand>(session) });
    m_commands.insert({ "torrents.resume",       std::make_shared<pika::RPC::TorrentsResumeCommand>(session) });
}

void JsonRpcHandler::Execute(std::shared_ptr<HttpRequestHandler::Context> context)
{
    namespace http = boost::beast::http;

    auto const response = [context](std::string const& body)
    {
        http::response<http::string_body> res{http::status::ok, context->Request().version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "application/json");
        res.keep_alive(context->Request().keep_alive());
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
        auto command = m_commands.find(method);

        if (command == m_commands.end())
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
        request = json::parse(context->Request().body());
    }
    catch (std::exception const& ex)
    {
        context->Write(response(
            buildError(-32700, "Parse error", json(ex.what())).dump()));
        return;
    }

    if (request.is_array())
    {
        json batch = json::array();

        for (auto& req : request)
        {
            batch.push_back(executeRequest(req));
        }

        context->Write(response(batch.dump()));
        return;
    }
    else if (request.is_object())
    {
        context->Write(response(
            createResponse(
                executeRequest(request)).dump()));
        return;
    }

    context->Write(response(buildError(-32600, "Invalid request").dump()));
}
