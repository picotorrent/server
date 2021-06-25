#pragma once

#include <filesystem>
#include <memory>
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

    private:
        std::filesystem::path m_databaseFilePath;
        boost::log::trivial::severity_level m_logLevel;
        std::string m_host;
        uint16_t m_port;
        std::shared_ptr<std::string> m_webRoot;
    };
}
