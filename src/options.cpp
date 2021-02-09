#include "options.hpp"

namespace fs = std::filesystem;
using pt::Server::Options;

std::shared_ptr<Options> Options::Load(int argc, char* argv[])
{
    auto opts = std::make_shared<Options>();
    opts->m_databaseFilePath = fs::path(argv[0]).parent_path() / "PicoTorrent.sqlite";
    opts->m_host = "127.0.0.1";
    opts->m_port = 1337;
    opts->m_webRoot = nullptr;

    if (const char* httpHost = std::getenv("PICOTORRENT_HTTP_HOST"))
    {
        opts->m_host = httpHost;
    }

    if (const char* webRoot = std::getenv("PICOTORRENT_WEBROOT_PATH"))
    {
        opts->m_webRoot = std::make_shared<std::string>(webRoot);
    }

    return opts;
}
