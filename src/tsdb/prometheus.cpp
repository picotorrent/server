#include "prometheus.hpp"

#include <regex>

using pt::Server::TSDB::Prometheus;

Prometheus::~Prometheus() = default;

void Prometheus::Execute(std::shared_ptr<Http::HttpRequestHandler::Context> context)
{
    namespace http = boost::beast::http;

    auto const response = [context](std::string const& body)
    {
        http::response<http::string_body> res{http::status::ok, context->Request().version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain; version=0.0.4");
        res.keep_alive(context->Request().keep_alive());
        res.body() = body;
        res.prepare_payload();
        return res;
    };

    if (m_metrics.has_value() && m_timestamp.has_value())
    {
        std::stringstream ss;

        for (auto const& metric : m_metrics.value())
        {
            ss << std::regex_replace(metric.first, std::regex("\\."), "_") << " " << metric.second << "\n";
        }

        context->Write(response(ss.str()));
    }
}

void Prometheus::WriteMetrics(const std::map<std::string, int64_t> &metrics, std::chrono::milliseconds timestamp)
{
    m_metrics = metrics;
    m_timestamp = timestamp;
}
