#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <toml++/toml.h>

namespace pika
{
    struct Options
    {
        static std::shared_ptr<Options> Load(int argc, char* argv[]);

        toml::table Config() { return m_config; }
        std::filesystem::path DatabaseFilePath() { return m_databaseFilePath; }
        boost::log::trivial::severity_level LogLevel() { return m_logLevel; }
        boost::asio::ip::tcp::endpoint HttpEndpoint() const { return m_httpEndpoint; }
        std::vector<std::filesystem::path> Plugins() const { return m_plugins; }

    private:
        toml::table m_config;
        std::filesystem::path m_databaseFilePath;
        boost::log::trivial::severity_level m_logLevel;
        boost::asio::ip::tcp::endpoint m_httpEndpoint;
        std::vector<std::filesystem::path> m_plugins;
    };
}
