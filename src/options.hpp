#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

namespace pika
{
    struct Options
    {
        static std::shared_ptr<Options> Load(int argc, char* argv[]);

        std::filesystem::path DatabaseFilePath() { return m_databaseFilePath; }
        boost::log::trivial::severity_level LogLevel() { return m_logLevel; }
        boost::asio::ip::tcp::endpoint HttpEndpoint() const { return m_httpEndpoint; }

        bool PrometheusExporterEnabled() const { return m_prometheusEnabled; }

    private:
        std::filesystem::path m_databaseFilePath;
        boost::log::trivial::severity_level m_logLevel;
        boost::asio::ip::tcp::endpoint m_httpEndpoint;

        bool m_prometheusEnabled;
    };
}
