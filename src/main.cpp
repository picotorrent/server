#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <sqlite3.h>

#include "config.hpp"
#include "data/migrator.hpp"
#include "log.hpp"
#include "session.hpp"

#include "http/handlers/jsonrpchandler.hpp"
#include "http/httplistener.hpp"
#include "plugins/pluginfactory.hpp"

namespace fs = std::filesystem;
namespace lt = libtorrent;

using pika::Data::Migrator;
using pika::Http::Handlers::JsonRpcHandler;
using pika::Http::HttpListener;
using pika::Log;
using pika::Session;

struct sqlite3_deleter
{
    void operator()(sqlite3* db)
    {
        BOOST_LOG_TRIVIAL(info)
            << "Running SQLite VACUUM with result: "
            << sqlite3_exec(db, "VACUUM;", nullptr, nullptr, nullptr);

        sqlite3_close(db);
    }
};

static std::unique_ptr<sqlite3, sqlite3_deleter> OpenSQLiteDatabase(const char* path)
{
    BOOST_LOG_TRIVIAL(info) << "Loading database from " << path;

    sqlite3* buffer = nullptr;
    return sqlite3_open(path, &buffer) == SQLITE_OK
        ? std::unique_ptr<sqlite3, sqlite3_deleter>(buffer)
        : throw std::runtime_error("failed to open database");
}

struct App
{
    explicit App(int argc, char* argv[])
        : m_argc(argc)
        , m_argv(argv)
    {
    }

    ~App() { BOOST_LOG_TRIVIAL(info) << "Pika shutting down."; }

    toml::table Bootstrap()
    {
        auto tbl = pika::Config::Load(m_argc, m_argv);

        auto boostLevel = boost::log::trivial::info;
        auto level = tbl["log_level"].value<std::string>().value_or("info");

        if (level == "trace")   { boostLevel = boost::log::trivial::trace; }
        if (level == "debug")   { boostLevel = boost::log::trivial::debug; }
        if (level == "info")    { boostLevel = boost::log::trivial::info; }
        if (level == "warning") { boostLevel = boost::log::trivial::warning; }
        if (level == "error")   { boostLevel = boost::log::trivial::error; }
        if (level == "fatal")   { boostLevel = boost::log::trivial::fatal; }

        Log::Setup(boostLevel);

        return tbl;
    }

    int Run(const toml::table& config)
    {
        BOOST_LOG_TRIVIAL(info) << "Pika starting up";

        auto db = OpenSQLiteDatabase(config["db"].value<const char*>().value_or("pika.sqlite"));
        sqlite3_exec(db.get(), "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);

        if (!Migrator::Run(db.get()))
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to migrate database, shutting down";
            return 1;
        }

        boost::asio::io_context io;
        boost::asio::signal_set signals(io, SIGINT, SIGTERM);
        signals.async_wait(
                [&io](boost::system::error_code const& ec, int signal)
                {
                    BOOST_LOG_TRIVIAL(info) << "Interrupt received (" << signal << ") - stopping...";
                    io.stop();
                });

        auto sm = Session::Load(io, db.get(), config);
        auto http = std::make_shared<HttpListener>(io, config);

        http->AddHandler("POST", "/api/jsonrpc", std::make_shared<JsonRpcHandler>(db.get(), sm));

        auto pf = std::make_shared<pika::Plugins::PluginFactory>(io, config, http);

        //for (const auto& path : options->Plugins())
        //{
        //    pf->Load(path);
        //}

        sm->AddEventHandler(pf);
        http->Run();

        io.run();

        return 0;
    }

private:
    int m_argc;
    char** m_argv;
};

int main(int argc, char* argv[])
{
    App app(argc, argv);
    return app.Run(app.Bootstrap());
}
