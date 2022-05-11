#include "webroothandler.hpp"

#include <filesystem>
#include <utility>

#include "../mimetype.hpp"

namespace fs = std::filesystem;
using pika::Http::Handlers::WebRootHandler;

WebRootHandler::WebRootHandler(std::string root, std::string virtualPath)
    : m_root(std::move(root))
    , m_virtualPath(std::move(virtualPath))
{
}

void WebRootHandler::Execute(std::shared_ptr<HttpRequestHandler::Context> context)
{
    namespace http = boost::beast::http;

    std::string unprefixed = context->Request().target().substr(m_virtualPath.size()).to_string();
    fs::path path = fs::path(m_root).concat(unprefixed);

    if (unprefixed.empty()) { unprefixed = "/"; }
    if (unprefixed.back() == '/') { path /= "index.html"; }

    boost::beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), boost::beast::file_mode::scan, ec);

    auto const not_found = [context](boost::beast::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, context->Request().version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(context->Request().keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    auto const server_error = [&context](boost::beast::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, context->Request().version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(context->Request().keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    if(ec == boost::beast::errc::no_such_file_or_directory)
    {
        context->Write(not_found(context->Request().target()));
    }
    else if (ec)
    {
        context->Write(server_error(ec.message()));
    }
    else
    {
        auto const size = body.size();

        http::response<http::file_body> res{
                std::piecewise_construct,
                std::make_tuple(std::move(body)),
                std::make_tuple(http::status::ok, context->Request().version())};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, MimeType(path.string()));
        res.content_length(size);
        res.keep_alive(context->Request().keep_alive());

        context->Write(std::move(res));
    }
}
