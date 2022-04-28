#include "options.hpp"

#include <boost/program_options.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

using pt::Server::Options;

std::shared_ptr<Options> Options::Load(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("http-addr", po::value<std::string>(), "set the http server address")
        ("http-port", po::value<int>(), "set the http server port")
        ("log-level", po::value<std::string>(), "set log level")
        // Prometheus options
        ("prometheus-exporter", "enable the Prometheus metrics exporter")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    auto opts = std::make_shared<Options>();
    opts->m_databaseFilePath = fs::path(argv[0]).parent_path() / "PicoTorrent.sqlite";
    opts->m_host = "127.0.0.1";
    opts->m_port = 1337;
    opts->m_logLevel = boost::log::trivial::info;
    opts->m_webRoot = nullptr;

    if (const char* dbPath = std::getenv("PICOTORRENT_DB_FILE"))
    {
        opts->m_databaseFilePath = fs::path(dbPath);
    }

    if (const char* httpHost = std::getenv("PICOTORRENT_HTTP_HOST"))
    {
        opts->m_host = httpHost;
    }

    if (const char* httpPort = std::getenv("PICOTORRENT_HTTP_PORT"))
    {
        opts->m_port = std::stoi(httpPort);
    }

    if (const char* webRoot = std::getenv("PICOTORRENT_WEBROOT_PATH"))
    {
        opts->m_webRoot = std::make_shared<std::string>(webRoot);
    }

    if (vm.count("http-addr")) { opts->m_host = vm["http-addr"].as<std::string>(); }
    if (vm.count("http-port")) { opts->m_port = vm["http-port"].as<int>(); }

    if (vm.count("log-level"))
    {
        std::string level = vm["log-level"].as<std::string>();
        if (level == "trace") { opts->m_logLevel = boost::log::trivial::trace; }
        if (level == "debug") { opts->m_logLevel = boost::log::trivial::debug; }
        if (level == "info") { opts->m_logLevel = boost::log::trivial::info; }
        if (level == "warning") { opts->m_logLevel = boost::log::trivial::warning; }
        if (level == "error") { opts->m_logLevel = boost::log::trivial::error; }
        if (level == "fatal") { opts->m_logLevel = boost::log::trivial::fatal; }
    }

    if (std::getenv("PICOTORRENT_PROMETHEUS_EXPORTER")) { opts->m_prometheusEnabled = true; }
    if (vm.count("prometheus-exporter")) { opts->m_prometheusEnabled = true; }

    return opts;
}
