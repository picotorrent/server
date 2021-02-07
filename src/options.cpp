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

    return opts;
}
