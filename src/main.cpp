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
#include "rpc/configget.hpp"
#include "rpc/configset.hpp"
#include "rpc/listeninterfacescreate.hpp"
#include "rpc/listeninterfacesgetall.hpp"
#include "rpc/listeninterfacesremove.hpp"
#include "rpc/profilesgetactive.hpp"
#include "rpc/profilesgetall.hpp"
#include "rpc/profilesupdate.hpp"
#include "rpc/proxycreate.hpp"
#include "rpc/proxygetall.hpp"
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
using pt::Server::RPC::ConfigGetCommand;
using pt::Server::RPC::ConfigSetCommand;
using pt::Server::RPC::ListenInterfacesCreateCommand;
using pt::Server::RPC::ListenInterfacesGetAllCommand;
using pt::Server::RPC::ListenInterfacesRemoveCommand;
using pt::Server::RPC::ProfilesGetActiveCommand;
using pt::Server::RPC::ProfilesGetAllCommand;
using pt::Server::RPC::ProfilesUpdateCommand;
using pt::Server::RPC::ProxyCreateCommand;
using pt::Server::RPC::ProxyGetAllCommand;
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

    http->Commands().insert({ "config.get",              std::make_shared<ConfigGetCommand>(db) });
    http->Commands().insert({ "config.set",              std::make_shared<ConfigSetCommand>(db) });
    http->Commands().insert({ "listenInterfaces.create", std::make_shared<ListenInterfacesCreateCommand>(db, sm) });
    http->Commands().insert({ "listenInterfaces.getAll", std::make_shared<ListenInterfacesGetAllCommand>(db) });
    http->Commands().insert({ "listenInterfaces.remove", std::make_shared<ListenInterfacesRemoveCommand>(db, sm) });
    http->Commands().insert({ "profiles.getActive",      std::make_shared<ProfilesGetActiveCommand>(db) });
    http->Commands().insert({ "profiles.getAll",         std::make_shared<ProfilesGetAllCommand>(db) });
    http->Commands().insert({ "profiles.update",         std::make_shared<ProfilesUpdateCommand>(db, sm) });
    http->Commands().insert({ "proxy.create",            std::make_shared<ProxyCreateCommand>(db, sm) });
    http->Commands().insert({ "proxy.getAll",            std::make_shared<ProxyGetAllCommand>(db) });
    http->Commands().insert({ "session.addTorrent",      std::make_shared<SessionAddTorrentCommand>(sm) });
    http->Commands().insert({ "session.removeTorrent",   std::make_shared<SessionRemoveTorrentCommand>(sm) });
    http->Commands().insert({ "settingsPack.create",     std::make_shared<SettingsPackCreateCommand>(db) });
    http->Commands().insert({ "settingsPack.getById",    std::make_shared<SettingsPackGetByIdCommand>(db) });
    http->Commands().insert({ "settingsPack.list",       std::make_shared<SettingsPackList>(db) });
    http->Commands().insert({ "settingsPack.update",     std::make_shared<SettingsPackUpdateCommand>(db, sm) });
    http->Commands().insert({ "torrents.pause",          std::make_shared<TorrentsPauseCommand>(sm) });
    http->Commands().insert({ "torrents.resume",         std::make_shared<TorrentsResumeCommand>(sm) });
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
