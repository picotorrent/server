#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <sqlite3.h>

#include "database.hpp"
#include "log.hpp"
#include "options.hpp"
#include "sessionmanager.hpp"

#include "http/handlers/jsonrpchandler.hpp"
#include "http/handlers/websockethandler.hpp"
#include "http/httplistener.hpp"
#include "scripting/scripthost.hpp"
#include "tsdb/influxdb.hpp"
#include "tsdb/prometheus.hpp"
#include "tsdb/timeseriesdatabase.hpp"

namespace fs = std::filesystem;
namespace lt = libtorrent;

using pt::Server::Database;
using pt::Server::Http::Handlers::JsonRpcHandler;
using pt::Server::Http::Handlers::WebSocketHandler;
using pt::Server::Http::HttpListener;
using pt::Server::Log;
using pt::Server::Options;
using pt::Server::Scripting::ScriptHost;
using pt::Server::SessionManager;

void Run(sqlite3* db, std::shared_ptr<Options> const& options)
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
        boost::asio::ip::tcp::endpoint
        {
            boost::asio::ip::make_address(options->Host()),
            options->Port()
        },
        options->WebRoot());

    if (options->IsValidInfluxDbConfig())
    {
        BOOST_LOG_TRIVIAL(info) << "InfluxDb configuration seems legit. Configuring reporter...";

        tsdb = std::make_shared<pt::Server::TSDB::InfluxDb>(
            io,
            options->InfluxDbHost().value(),
            options->InfluxDbPort().value(),
            options->InfluxDbOrganization().value(),
            options->InfluxDbBucket().value(),
            options->InfluxDbToken().value());
    }
    else if (options->PrometheusExporterEnabled())
    {
        BOOST_LOG_TRIVIAL(info) << "Enabling Prometheus metrics exporter";

        auto prometheus = std::make_shared<pt::Server::TSDB::Prometheus>();
        http->AddHandler("GET", "/metrics", prometheus);
        tsdb = prometheus;
    }

    auto sm = SessionManager::Load(io, db, tsdb);

    http->AddHandler("POST", "/api/jsonrpc", std::make_shared<JsonRpcHandler>(db, sm));
    http->AddHandler("GET", "/api/ws", std::make_shared<WebSocketHandler>(sm));
    http->Run();

    io.run();
}

int main(int argc, char* argv[])
{
    auto options = Options::Load(argc, argv);
    if (!options) { return 1; }

    Log::Setup(options->LogLevel());

    BOOST_LOG_TRIVIAL(info) << "PicoTorrent Server starting up...";
    BOOST_LOG_TRIVIAL(info) << "Opening database from " << options->DatabaseFilePath();

    sqlite3* db = nullptr;
    sqlite3_open(options->DatabaseFilePath().c_str(), &db);

    if (!Database::Migrate(db))
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to migrate database, shutting down";
        return 1;
    }

    Run(db, options);

    BOOST_LOG_TRIVIAL(info) << "Closing database...";
    sqlite3_close(db);

    return 0;
}
