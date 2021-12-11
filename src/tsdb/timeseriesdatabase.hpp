#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <string>

namespace pt::Server::TSDB
{
    struct TimeSeriesDatabase
    {
        virtual ~TimeSeriesDatabase() = default;

        virtual void WriteMetrics(
            std::map<std::string, int64_t> const& metrics,
            std::chrono::milliseconds timestamp) = 0;
    };
}
