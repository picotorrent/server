#pragma once

#include "timeseriesdatabase.hpp"

#include <boost/asio.hpp>

namespace pt::Server::TSDB
{
    class InfluxDb : public TimeSeriesDatabase
    {
    public:
        InfluxDb(
            boost::asio::io_context& io,
            const std::string& host,
            int16_t port,
            const std::string& org,
            const std::string& bucket,
            const std::string& token);

        ~InfluxDb() override;

        void WriteMetrics(
            std::map<std::string, int64_t> const& metrics,
            std::chrono::milliseconds timestamp) override;

    private:
        class HttpClient;

        struct Metrics
        {
            std::map<std::string, int64_t> data;
            std::chrono::milliseconds timestamp;
        };

        void Send(boost::system::error_code ec);

        boost::asio::io_context& m_io;
        boost::asio::deadline_timer m_timer;
        std::shared_ptr<HttpClient> m_httpClient;
        std::vector<Metrics> m_current;
        std::vector<Metrics> m_inflight;
    };
}
