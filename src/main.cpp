#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <sqlite3.h>

#include "data/migrator.hpp"
#include "log.hpp"
#include "options.hpp"
#include "session.hpp"

#include "eventhandlers/sessionstatshandler.hpp"
#include "http/handlers/jsonrpchandler.hpp"
#include "http/handlers/metricshandler.hpp"
#include "http/httplistener.hpp"
#include "scripting/engine.hpp"

namespace fs = std::filesystem;
namespace lt = libtorrent;

using pika::Data::Migrator;
using pika::Http::Handlers::JsonRpcHandler;
using pika::Http::Handlers::MetricsHandler;
using pika::Http::HttpListener;
using pika::Log;
using pika::Options;
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

    std::shared_ptr<Options> Bootstrap()
    {
        if (auto opts = Options::Load(m_argc, m_argv))
        {
            Log::Setup(opts->LogLevel());
            return opts;
        }

        return nullptr;
    }

    int Run(const std::shared_ptr<Options> &options)
    {
        auto db = OpenSQLiteDatabase(options->DatabaseFilePath().c_str());

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

        pika::Scripting::Engine se;
        se.Run();

        auto ssh = std::make_shared<pika::EventHandlers::SessionStatsHandler>();

        auto sm = Session::Load(io, db.get());
        sm->AddEventHandler(ssh);

        auto http = std::make_shared<HttpListener>(io, options->HttpEndpoint());
        http->AddHandler("POST", "/api/jsonrpc", std::make_shared<JsonRpcHandler>(db.get(), sm));

        if (options->PrometheusExporterEnabled())
        {
            http->AddHandler("GET", "/metrics", std::make_shared<MetricsHandler>(ssh));
        }

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

    if (auto options = app.Bootstrap())
    {
        BOOST_LOG_TRIVIAL(info) << "Pika starting up...";
        return app.Run(options);
    }

    return 1;
}
