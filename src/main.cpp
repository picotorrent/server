#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <sqlite3.h>

#include "data/migrator.hpp"
#include "log.hpp"
#include "options.hpp"
#include "session.hpp"

#include "http/handlers/jsonrpchandler.hpp"
#include "http/handlers/websockethandler.hpp"
#include "http/httplistener.hpp"
#include "tsdb/prometheus.hpp"
#include "tsdb/timeseriesdatabase.hpp"

namespace fs = std::filesystem;
namespace lt = libtorrent;

using pt::Server::Data::Migrator;
using pt::Server::Http::Handlers::JsonRpcHandler;
using pt::Server::Http::Handlers::WebSocketHandler;
using pt::Server::Http::HttpListener;
using pt::Server::Log;
using pt::Server::Options;
using pt::Server::Session;

struct sqlite3_deleter
{
    void operator()(sqlite3* db)
    {
        sqlite3_close(db);
    }
};

static std::unique_ptr<sqlite3, sqlite3_deleter> OpenSQLiteDatabase(const char* path)
{
    sqlite3* buffer = nullptr;
    int err = sqlite3_open(path, &buffer);
    if (err)
    {
        throw std::runtime_error("failed to open sqlite");
    }
    return std::unique_ptr<sqlite3, sqlite3_deleter>(buffer);
}

static int Run(sqlite3* db, std::shared_ptr<Options> const& options)
{
    boost::asio::io_context io;
    boost::asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(
        [&io](boost::system::error_code const& ec, int signal)
        {
            io.stop();
        });

    std::shared_ptr<pt::Server::TSDB::TimeSeriesDatabase> tsdb = nullptr;

    auto http = std::make_shared<HttpListener>(
        io,
        options->HttpEndpoint(),
        options->WebRoot());

    if (options->PrometheusExporterEnabled())
    {
        BOOST_LOG_TRIVIAL(info) << "Enabling Prometheus metrics exporter";

        auto prometheus = std::make_shared<pt::Server::TSDB::Prometheus>();
        http->AddHandler("GET", "/metrics", prometheus);
        tsdb = prometheus;
    }

    auto sm = Session::Load(io, db, tsdb);

    http->AddHandler("POST", "/api/jsonrpc", std::make_shared<JsonRpcHandler>(db, sm));
    http->AddHandler("GET", "/api/ws", std::make_shared<WebSocketHandler>(sm));
    http->Run();

    io.run();

    return 0;
}

int main(int argc, char* argv[])
{
    auto options = Options::Load(argc, argv);
    if (!options) { return 1; }

    Log::Setup(options->LogLevel());

    BOOST_LOG_TRIVIAL(info) << "PicoTorrent Server starting up...";
    BOOST_LOG_TRIVIAL(info) << "Opening database from " << options->DatabaseFilePath();

    auto db = OpenSQLiteDatabase(options->DatabaseFilePath().c_str());

    if (!Migrator::Run(db.get()))
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to migrate database, shutting down";
        return 1;
    }

    return Run(db.get(), options);
}
