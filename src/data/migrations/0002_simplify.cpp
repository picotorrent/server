#include "0002_simplify.hpp"

#include <libtorrent/fingerprint.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/settings_pack.hpp>

#include "../models/listeninterface.hpp"
#include "../models/profile.hpp"
#include "../models/proxy.hpp"
#include "../models/settingspack.hpp"
#include "../statement.hpp"

namespace lt = libtorrent;
using pt::Server::Data::Migrations::Simplify;

int Simplify::Migrate(sqlite3 *db)
{
    // delete existing settings packs
    int res = sqlite3_exec(db, "DELETE FROM settings_pack", nullptr, nullptr, nullptr);

    if (res != SQLITE_OK)
    {
        return res;
    }

    // add all settings pack columns
    res = sqlite3_exec(
        db,
        // string types
        "ALTER TABLE settings_pack ADD COLUMN outgoing_interfaces TEXT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN listen_interfaces   TEXT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_hostname      TEXT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_username      TEXT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_password      TEXT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN i2p_hostname        TEXT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN dht_bootstrap_nodes TEXT NULL;\n"

        // bool types
        "ALTER TABLE settings_pack ADD COLUMN socks5_udp_send_local_ep  INTEGER NOT NULL CHECK(socks5_udp_send_local_ep  IN (0,1));\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_hostnames           INTEGER NOT NULL CHECK(proxy_hostnames           IN (0,1));\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_peer_connections    INTEGER NOT NULL CHECK(proxy_peer_connections    IN (0,1));\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_tracker_connections INTEGER NOT NULL CHECK(proxy_tracker_connections IN (0,1));\n"

        // int types
        "ALTER TABLE settings_pack ADD COLUMN alert_mask           INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_type           INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN proxy_port           INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN i2p_port             INTEGER NOT NULL;\n"
        "ALTER TABLE settings_pack ADD COLUMN metadata_token_limit INTEGER NOT NULL;\n"
        ,
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK)
    {
        return res;
    }

    auto SetDefaults = [](lt::settings_pack sp)
    {
        sp.set_int(lt::settings_pack::alert_mask, lt::alert_category::all);
        sp.set_str(lt::settings_pack::peer_fingerprint, lt::generate_fingerprint("PS", 0, 1));
        return sp;
    };

    // insert default settings pack
    Data::SettingsPack::Create(db, "Default", "Default settings", SetDefaults(lt::default_settings()));

    // get active profile
    auto profile = Data::Models::Profile::GetActive(db);

    if (profile->ProxyId().has_value())
    {
        // update proxy settings for default settings pack
        auto proxy = Data::Models::Proxy::GetById(db, profile->ProxyId().value());

        Data::Statement::ForEach(
            db,
            "UPDATE settings_pack SET proxy_hostname = $1, proxy_port = $2, proxy_type = $3, proxy_username = $4, proxy_password = $5, proxy_hostnames = $6, proxy_peer_connections = $7, proxy_tracker_connections = $8 WHERE name = 'Default'",
            [](auto const&) {},
            [&proxy](sqlite3_stmt* stmt)
            {
                sqlite3_bind_text(stmt, 1, proxy->Hostname().c_str(), static_cast<int>(proxy->Hostname().size()), SQLITE_TRANSIENT);
                sqlite3_bind_int(stmt,  2, proxy->Port());
                sqlite3_bind_int(stmt,  3, proxy->Type());

                if (proxy->Username().has_value())
                {
                    sqlite3_bind_text(stmt, 4, proxy->Username().value().c_str(), static_cast<int>(proxy->Username().value().size()), SQLITE_TRANSIENT);
                }
                else
                {
                    sqlite3_bind_null(stmt, 4);
                }

                if (proxy->Password().has_value())
                {
                    sqlite3_bind_text(stmt, 5, proxy->Password().value().c_str(), static_cast<int>(proxy->Password().value().size()), SQLITE_TRANSIENT);
                }
                else
                {
                    sqlite3_bind_null(stmt, 5);
                }

                sqlite3_bind_int(stmt, 6, proxy->ProxyHostnames() ? 1 : 0);
                sqlite3_bind_int(stmt, 7, proxy->ProxyPeerConnections() ? 1 : 0);
                sqlite3_bind_int(stmt, 8, proxy->ProxyTrackerConnections() ? 1 : 0);
            });
    }

    std::stringstream incoming;
    std::stringstream outgoing;

    for (auto const& listenInterface : Data::Models::ListenInterface::GetAll(db))
    {
        incoming << ","
                 << listenInterface->Host()
                 << ":"
                 << listenInterface->Port()
                 << (listenInterface->IsLocal() ? "l" : "")
                 << (listenInterface->IsSsl() ? "s" : "");

        if (listenInterface->IsOutgoing())
        {
            outgoing << ","
                     << listenInterface->Host();
        }
    }

    if (!incoming.str().empty())
    {
        Data::Statement::ForEach(
            db,
            "UPDATE settings_pack SET listen_interfaces = $1 WHERE name = 'Default'",
            [](auto const&) {},
            [&incoming](sqlite3_stmt* stmt)
            {
                sqlite3_bind_text(stmt, 1, incoming.str().substr(1).c_str(), static_cast<int>(incoming.str().substr(1).size()), SQLITE_TRANSIENT);
            });
    }

    if (!outgoing.str().empty())
    {
        Data::Statement::ForEach(
            db,
            "UPDATE settings_pack SET outgoing_interfaces = $1 WHERE name = 'Default'",
            [](auto const&) {},
            [&outgoing](sqlite3_stmt* stmt)
            {
                sqlite3_bind_text(stmt, 1, outgoing.str().substr(1).c_str(), static_cast<int>(outgoing.str().substr(1).size()), SQLITE_TRANSIENT);
            });
    }

    res = sqlite3_exec(db, "DROP TABLE listen_interfaces", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    res = sqlite3_exec(db, "DROP TABLE profiles", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    res = sqlite3_exec(db, "DROP TABLE proxy", nullptr, nullptr, nullptr);
    if (res != SQLITE_OK) { return res; }

    return SQLITE_OK;
}
