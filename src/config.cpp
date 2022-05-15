#include "config.hpp"

#include <filesystem>
#include <fstream>

#include <boost/program_options.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

using pika::Config;

toml::table Config::Load(int argc, char **argv)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("config-file", po::value<std::string>(), "path to the pika.toml config file")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // Config load order
    // 1. CLI args
    // 2. Env PIKA_CONFIG
    // 3. Env PIKA_CONFIG_FILE
    // 4. Current working directory

    // Load config file from CLI args first, if if exist
    if (vm.count("config-file"))
    {
        std::ifstream cfg(vm["config-file"].as<std::string>(), std::ios::binary);
        return toml::parse(cfg);
    }

    if (const char* pikaConfig = std::getenv("PIKA_CONFIG"))
    {
        return toml::parse(pikaConfig);
    }

    if (const char* pikaConfigFile = std::getenv("PIKA_CONFIG_FILE"))
    {
        std::ifstream cfg(pikaConfigFile, std::ios::binary);
        return toml::parse(cfg);
    }

    fs::path p = fs::current_path() / "pika.toml";

    if (fs::exists(p) && fs::is_regular_file(p))
    {
        std::ifstream cfg(p, std::ios::binary);
        return toml::parse(cfg);
    }

    return {};
}
