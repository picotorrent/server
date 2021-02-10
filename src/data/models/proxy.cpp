#include "proxy.hpp"

#include <boost/log/trivial.hpp>

#include "../sqliteexception.hpp"

using pt::Server::Data::SQLiteException;
using pt::Server::Data::Models::Proxy;

std::shared_ptr<Proxy> Proxy::Create(
    sqlite3* db,
    std::string const& name,
    libtorrent::settings_pack::proxy_type_t type,
    std::string const& hostname,
    int port,
    std::optional<std::string> const& username,
    std::optional<std::string> const& password,
    bool proxyHostnames,
    bool proxyPeerConnections,
    bool proxyTrackerConnections)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare_v2(
        db,
        "INSERT INTO proxy (name,type,hostname,port,username,password,proxy_hostnames,proxy_peer_connections,proxy_tracker_connections) "
        "VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9);",
        -1,
        &stmt,
        nullptr);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to prepare SQL for inserting proxy: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), static_cast<int>(name.size()), SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, static_cast<int>(type));
    sqlite3_bind_text(stmt, 3, hostname.c_str(), static_cast<int>(hostname.size()), SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, port);

    if (username)
    {
        std::string user = username.value();
        sqlite3_bind_text(stmt, 5, user.c_str(), static_cast<int>(user.size()), SQLITE_TRANSIENT);
    }
    else
    {
        sqlite3_bind_null(stmt, 5);
    }

    if (password)
    {
        std::string pass = password.value();
        sqlite3_bind_text(stmt, 6, pass.c_str(), static_cast<int>(pass.size()), SQLITE_TRANSIENT);
    }
    else
    {
        sqlite3_bind_null(stmt, 6);
    }

    sqlite3_bind_int(stmt, 7, proxyHostnames          ? 1 : 0);
    sqlite3_bind_int(stmt, 8, proxyPeerConnections    ? 1 : 0);
    sqlite3_bind_int(stmt, 9, proxyTrackerConnections ? 1 : 0);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to insert proxy: " << sqlite3_errmsg(db);
        throw SQLiteException();
    }

    sqlite3_finalize(stmt);

    auto result = std::make_shared<Proxy>();
    result->m_id = static_cast<int>(sqlite3_last_insert_rowid(db));

    return std::move(result);
}

std::shared_ptr<Proxy> Proxy::GetById(sqlite3* db, int proxyId)
{
    return nullptr;
}
