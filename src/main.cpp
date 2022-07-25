#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <sqlite3.h>

#include "config.hpp"
#include "data/migrator.hpp"
#include "log.hpp"
#include "session.hpp"

#include "http/handlers/eventshandler.hpp"
#include "http/handlers/jsonrpchandler.hpp"

#include <libpika/http/httpfuncs.hpp>
#include <libpika/http/server.hpp>
#include <libpika/jsonrpc/jsonrpcserver.hpp>

#include "rpc/configget.hpp"
#include "rpc/configset.hpp"
#include "rpc/sessionaddtorrent.hpp"
#include "rpc/sessiongettorrents.hpp"
#include "rpc/sessionfindtorrents.hpp"
#include "rpc/sessionremovetorrent.hpp"
#include "rpc/sessionstats.hpp"
#include "rpc/torrentsfilesget.hpp"
#include "rpc/torrentslabelsget.hpp"
#include "rpc/torrentsmovestorage.hpp"
#include "rpc/torrentspause.hpp"
#include "rpc/torrentsresume.hpp"
#include "rpc/torrentssetlabels.hpp"

namespace fs = std::filesystem;
namespace lt = libtorrent;

using pika::Data::Migrator;
using pika::Http::Handlers::EventsHandler;
using pika::Http::Handlers::JsonRpcHandler;
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

int main(int argc, char* argv[])
{
    auto tbl = pika::Config::Load(argc, argv);

    auto boostLevel = boost::log::trivial::info;
    auto level = tbl["log_level"].value<std::string>().value_or("info");

    if (level == "trace")   { boostLevel = boost::log::trivial::trace; }
    if (level == "debug")   { boostLevel = boost::log::trivial::debug; }
    if (level == "info")    { boostLevel = boost::log::trivial::info; }
    if (level == "warning") { boostLevel = boost::log::trivial::warning; }
    if (level == "error")   { boostLevel = boost::log::trivial::error; }
    if (level == "fatal")   { boostLevel = boost::log::trivial::fatal; }

    Log::Setup(boostLevel);

    BOOST_LOG_TRIVIAL(info) << "Pika starting up";

    auto db = OpenSQLiteDatabase(tbl["db"].value<const char*>().value_or("pika.sqlite"));
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

    pika::Session session(io, db.get(), pika::Session::Options{
        .settings = lt::default_settings()
    });

    libpika::jsonrpc::JsonRpcServer rpcServer({
        { "config.get", std::make_shared<pika::RPC::ConfigGetCommand>(db.get()) },
        { "config.set", std::make_shared<pika::RPC::ConfigSetCommand>(db.get()) },
        { "session.addTorrent", std::make_shared<pika::RPC::SessionAddTorrentCommand>(session) },
        { "session.findTorrents", std::make_shared<pika::RPC::SessionFindTorrents>(session) },
        { "session.getTorrents", std::make_shared<pika::RPC::SessionGetTorrentsCommand>(session) },
        { "session.removeTorrent", std::make_shared<pika::RPC::SessionRemoveTorrentCommand>(session) },
        { "session.stats", std::make_shared<pika::RPC::SessionStatsCommand>(session) },
        { "torrents.files.get", std::make_shared<pika::RPC::TorrentsFilesGetCommand>(session) },
        { "torrents.labels.get", std::make_shared<pika::RPC::TorrentsLabelsGetCommand>(db.get(), session) },
        { "torrents.labels.set", std::make_shared<pika::RPC::TorrentsLabelsSetCommand>(db.get(), session) },
        { "torrents.moveStorage", std::make_shared<pika::RPC::TorrentsMoveStorageCommand>(session) },
        { "torrents.pause", std::make_shared<pika::RPC::TorrentsPauseCommand>(session) },
        { "torrents.resume", std::make_shared<pika::RPC::TorrentsResumeCommand>(session) }
    });

    libpika::http::HttpServer httpServer(io, libpika::http::HttpServer::Options{
        .host = "127.0.0.1",
        .port = 1338
    });

    httpServer.Use(libpika::http::HttpGet("/api/events", EventsHandler(io, session)));
    httpServer.Use(libpika::http::HttpPost("/api/jsonrpc", JsonRpcHandler(rpcServer)));

    io.run();

    return 0;
}
