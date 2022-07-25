#include "jsonrpchandler.hpp"

#include <boost/beast.hpp>
#include <libpika/jsonrpc/jsonrpcserver.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using pika::Http::Handlers::JsonRpcHandler;

JsonRpcHandler::JsonRpcHandler(libpika::jsonrpc::JsonRpcServer& rpcServer)
    : m_rpc(rpcServer)
{
}

void JsonRpcHandler::operator()(const std::shared_ptr<libpika::http::Context>& context)
{
    namespace http = boost::beast::http;

    auto const response = [context](std::string const& body)
    {
        http::response<http::string_body> res{http::status::ok, context->Request().version()};
        res.set(http::field::server, "pika/1.0");
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

    context->WriteJson(m_rpc.Execute(request));
}
