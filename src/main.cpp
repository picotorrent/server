#include <filesystem>

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <libtorrent/session.hpp>
#include <sqlite3.h>

#include "database.hpp"
#include "log.hpp"
#include "options.hpp"
#include "sessionmanager.hpp"

#include "http/httplistener.hpp"
#include "rpc/profileslist.hpp"
#include "rpc/sessionaddtorrent.hpp"
#include "rpc/sessionremovetorrent.hpp"
#include "rpc/settingspackcreate.hpp"
#include "rpc/settingspackgetbyid.hpp"
#include "rpc/settingspacklist.hpp"
#include "rpc/settingspackupdate.hpp"
#include "rpc/torrentspause.hpp"
#include "rpc/torrentsresume.hpp"

namespace fs = std::filesystem;
namespace lt = libtorrent;

using pt::Server::Database;
using pt::Server::Http::HttpListener;
using pt::Server::Log;
using pt::Server::Options;
using pt::Server::RPC::ProfilesListCommand;
using pt::Server::RPC::SessionAddTorrentCommand;
using pt::Server::RPC::SessionRemoveTorrentCommand;
using pt::Server::RPC::SettingsPackCreateCommand;
using pt::Server::RPC::SettingsPackGetByIdCommand;
using pt::Server::RPC::SettingsPackList;
using pt::Server::RPC::SettingsPackUpdateCommand;
using pt::Server::RPC::TorrentsPauseCommand;
using pt::Server::RPC::TorrentsResumeCommand;
using pt::Server::SessionManager;

void Run(sqlite3* db, std::shared_ptr<Options> options)
{
    boost::asio::io_context io;
    boost::asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(
        [&io](boost::system::error_code const& ec, int signal)
        {
            io.stop();
        });

    auto sm = SessionManager::Load(io, db);
    auto http = std::make_shared<HttpListener>(
        io,
        boost::asio::ip::tcp::endpoint
        {
            boost::asio::ip::make_address(options->Host()),
            options->Port()
        },
        db,
        sm,
        options->WebRoot());

    http->Commands().insert({ "profiles.list",         std::make_shared<ProfilesListCommand>(db) });
    http->Commands().insert({ "session.addTorrent",    std::make_shared<SessionAddTorrentCommand>(sm) });
    http->Commands().insert({ "session.removeTorrent", std::make_shared<SessionRemoveTorrentCommand>(sm) });
    http->Commands().insert({ "settingsPack.create",   std::make_shared<SettingsPackCreateCommand>(db) });
    http->Commands().insert({ "settingsPack.getById",  std::make_shared<SettingsPackGetByIdCommand>(db) });
    http->Commands().insert({ "settingsPack.list",     std::make_shared<SettingsPackList>(db) });
    http->Commands().insert({ "settingsPack.update",   std::make_shared<SettingsPackUpdateCommand>(db, sm) });
    http->Commands().insert({ "torrents.pause",        std::make_shared<TorrentsPauseCommand>(sm) });
    http->Commands().insert({ "torrents.resume",       std::make_shared<TorrentsResumeCommand>(sm) });
    http->Run();

    io.run();
}

int main(int argc, char* argv[])
{
    Log::Setup();
    BOOST_LOG_TRIVIAL(info) << "PicoTorrent Server starting up...";

    auto options = Options::Load(argc, argv);
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
