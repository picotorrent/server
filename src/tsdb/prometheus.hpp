#pragma once

#include "../http/httprequesthandler.hpp"
#include "timeseriesdatabase.hpp"

#include <optional>

namespace pika::TSDB
{
    class Prometheus : public Http::HttpRequestHandler, public TimeSeriesDatabase
    {
    public:
        ~Prometheus() override;

        void Execute(std::shared_ptr<Http::HttpRequestHandler::Context> context) override;

        void WriteMetrics(
            std::map<std::string, int64_t> const& metrics,
            std::chrono::milliseconds timestamp) override;

    private:
        std::optional<std::map<std::string, int64_t>> m_metrics;
        std::optional<std::chrono::milliseconds> m_timestamp;
    };
}
