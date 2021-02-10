#include "listeninterface.hpp"

#include "../datareader.hpp"
#include "../sqliteexception.hpp"

using pt::Server::Data::SQLiteException;
using pt::Server::Data::Models::ListenInterface;

std::shared_ptr<ListenInterface> ListenInterface::Create(sqlite3* db, std::string const& host, int port, bool isLocal, bool isOutgoing, bool isSsl)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
        db,
        "INSERT INTO listen_interfaces (host, port, is_local, is_outgoing, is_ssl) VALUES ($1, $2, $3, $4, $5);",
        -1,
        &stmt,
        nullptr);

    sqlite3_bind_text(stmt, 1, host.c_str(), static_cast<int>(host.size()), SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, port);
    sqlite3_bind_int(stmt, 3, isLocal ? 1 : 0);
    sqlite3_bind_int(stmt, 4, isOutgoing ? 1 : 0);
    sqlite3_bind_int(stmt, 5, isSsl ? 1 : 0);

    switch (sqlite3_step(stmt))
    {
    case SQLITE_ERROR:
        throw SQLiteException();
        break;
    }

    sqlite3_finalize(stmt);

    auto res = std::make_shared<ListenInterface>();
    res->m_id         = static_cast<int>(sqlite3_last_insert_rowid(db));
    res->m_host       = host;
    res->m_port       = port;
    res->m_isLocal    = isLocal;
    res->m_isOutgoing = isOutgoing;
    res->m_isSsl      = isSsl;

    return std::move(res);
}

std::vector<std::shared_ptr<ListenInterface>> ListenInterface::GetAll(sqlite3* db)
{
    std::vector<std::shared_ptr<ListenInterface>> result;

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(
        db,
        "SELECT id,host,port,is_local,is_outgoing,is_ssl FROM listen_interfaces ORDER BY id ASC",
        -1,
        &stmt,
        nullptr);

    int res = SQLITE_ERROR;

    while ((res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        auto listenInterface = std::make_shared<ListenInterface>();
        listenInterface->m_id         = sqlite3_column_int(stmt, 0);
        listenInterface->m_host       = pt_sqlite3_column_std_string(stmt, 1).value();
        listenInterface->m_port       = sqlite3_column_int(stmt, 2);
        listenInterface->m_isLocal    = sqlite3_column_int(stmt, 3) == 1 ? true : false;
        listenInterface->m_isOutgoing = sqlite3_column_int(stmt, 4) == 1 ? true : false;
        listenInterface->m_isSsl      = sqlite3_column_int(stmt, 5) == 1 ? true : false;

        result.push_back(listenInterface);
    }

    return result;
}
