#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include <boost/log/trivial.hpp>

namespace pt::Server
{
    struct Options
    {
        static std::shared_ptr<Options> Load(int argc, char* argv[]);

        std::filesystem::path DatabaseFilePath() { return m_databaseFilePath; }
        boost::log::trivial::severity_level LogLevel() { return m_logLevel; }
        std::string Host() { return m_host; }
        uint16_t Port() { return m_port; }
        std::shared_ptr<std::string> WebRoot() { return m_webRoot; }

        bool PrometheusExporterEnabled() { return m_prometheusEnabled; }

        // InfluxDb options
        std::optional<std::string> InfluxDbHost() { return m_influxHost; }
        std::optional<uint16_t> InfluxDbPort() { return m_influxPort; }
        std::optional<std::string> InfluxDbBucket() { return m_influxBucket; }
        std::optional<std::string> InfluxDbOrganization() { return m_influxOrg; }
        std::optional<std::string> InfluxDbToken() { return m_influxToken; }

        bool IsValidInfluxDbConfig()
        {
            return m_influxHost.has_value()
                && m_influxPort.has_value()
                && m_influxBucket.has_value()
                && m_influxOrg.has_value()
                && m_influxToken.has_value();
        }

    private:
        std::filesystem::path m_databaseFilePath;
        boost::log::trivial::severity_level m_logLevel;
        std::string m_host;
        uint16_t m_port;
        std::shared_ptr<std::string> m_webRoot;

        bool m_prometheusEnabled;

        std::optional<std::string> m_influxHost;
        std::optional<uint16_t> m_influxPort;
        std::optional<std::string> m_influxBucket;
        std::optional<std::string> m_influxOrg;
        std::optional<std::string> m_influxToken;
    };
}
