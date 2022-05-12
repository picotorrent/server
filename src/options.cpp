#include "options.hpp"

#include <boost/program_options.hpp>
#include <toml++/toml.h>

namespace fs = std::filesystem;
namespace po = boost::program_options;

using pika::Options;

std::shared_ptr<Options> Options::Load(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("http-addr", po::value<std::string>(), "set the http server address")
        ("http-port", po::value<int>(), "set the http server port")
        ("log-level", po::value<std::string>(), "set log level")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    std::string httpHost = "127.0.0.1";
    int httpPort = 1337;

    auto opts = std::make_shared<Options>();
    opts->m_databaseFilePath = fs::path(argv[0]).parent_path() / "pika.sqlite";
    opts->m_logLevel = boost::log::trivial::info;

    if (fs::exists(fs::current_path() / "pika.toml"))
    {
        std::ifstream input(fs::current_path() / "pika.toml", std::ios::binary);
        opts->m_config = toml::parse(input);

        if (toml::array* plugins = opts->m_config["plugins"].as_array())
        {
            for (const auto& item : *plugins)
            {
                auto val = item.value<std::string>();
                if (val.has_value())
                {
                    opts->m_plugins.emplace_back(val.value());
                }
            }
        }
    }

    if (const char* dbPath = std::getenv("PIKA_DB_FILE"))
    {
        opts->m_databaseFilePath = fs::path(dbPath);
    }

    if (const char* httpHostEnv = std::getenv("PIKA_HTTP_HOST"))
    {
        httpHost = httpHostEnv;
    }

    if (const char* httpPortEnv = std::getenv("PIKA_HTTP_PORT"))
    {
        httpPort = std::stoi(httpPortEnv);
    }

    if (vm.count("http-addr")) { httpHost = vm["http-addr"].as<std::string>(); }
    if (vm.count("http-port")) { httpPort = vm["http-port"].as<int>(); }

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

    opts->m_httpEndpoint = boost::asio::ip::tcp::endpoint
        {
            boost::asio::ip::make_address(httpHost),
            static_cast<uint16_t>(httpPort)
        };

    return opts;
}
