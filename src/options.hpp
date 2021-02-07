#pragma once

#include <filesystem>
#include <memory>
#include <string>

namespace pt::Server
{
    struct Options
    {
        static std::shared_ptr<Options> Load(int argc, char* argv[]);

        std::filesystem::path DatabaseFilePath() { return m_databaseFilePath; }
        std::string Host() { return m_host; }
        uint16_t Port() { return m_port; }
        std::shared_ptr<std::string> WebRoot() { return m_webRoot; }

    private:
        std::filesystem::path m_databaseFilePath;
        std::string m_host;
        uint16_t m_port;
        std::shared_ptr<std::string> m_webRoot;
    };
}
