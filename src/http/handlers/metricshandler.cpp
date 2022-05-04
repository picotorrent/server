#include "metricshandler.hpp"

#include <regex>
#include <utility>

#include <boost/beast.hpp>

#include "../../eventhandlers/sessionstatshandler.hpp"

using pika::EventHandlers::SessionStatsHandler;
using pika::Http::Handlers::MetricsHandler;

MetricsHandler::MetricsHandler(std::weak_ptr<SessionStatsHandler> ssh)
    : m_ssh(std::move(ssh))
{
}

void MetricsHandler::Execute(std::shared_ptr<HttpRequestHandler::Context> context)
{
    namespace http = boost::beast::http;

    auto const response = [context](http::status status, std::string const& body)
    {
        http::response<http::string_body> res{status, context->Request().version()};
        res.set(http::field::server, "pika/1.0");
        res.set(http::field::content_type, "text/plain; version=0.0.4");
        res.keep_alive(context->Request().keep_alive());
        res.body() = body;
        res.prepare_payload();
        return res;
    };

    if (auto l = m_ssh.lock())
    {
        std::stringstream ss;

        for (auto const& metric : l->Stats())
        {
            ss
                << "pika_"
                << std::regex_replace(metric.first, std::regex("\\."), "_")
                << " "
                << metric.second
                << "\n";
        }

        return context->Write(response(http::status::ok, ss.str()));
    }

    context->Write(
        response(
            http::status::bad_request,
            "No metrics exporter found"));
}
