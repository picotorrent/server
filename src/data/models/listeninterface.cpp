#include "listeninterface.hpp"

#include "../datareader.hpp"
#include "../sqliteexception.hpp"
#include "../statement.hpp"

using pt::Server::Data::Models::ListenInterface;
using pt::Server::Data::SQLiteException;
using pt::Server::Data::Statement;

std::shared_ptr<ListenInterface> ListenInterface::Create(sqlite3* db, std::string const& host, int port, bool isLocal, bool isOutgoing, bool isSsl)
{
    Statement::ForEach(
        db,
        "INSERT INTO listen_interfaces (host, port, is_local, is_outgoing, is_ssl) VALUES ($1, $2, $3, $4, $5);",
        [](auto const&){},
        [&](sqlite3_stmt* stmt)
        {
            sqlite3_bind_text(stmt, 1, host.c_str(), static_cast<int>(host.size()), SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt,  2, port);
            sqlite3_bind_int(stmt,  3, isLocal ? 1 : 0);
            sqlite3_bind_int(stmt,  4, isOutgoing ? 1 : 0);
            sqlite3_bind_int(stmt,  5, isSsl ? 1 : 0);
        });

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

    Statement::ForEach(
        db,
        "SELECT id,host,port,is_local,is_outgoing,is_ssl FROM listen_interfaces ORDER BY id ASC",
        [&](Statement::Row const& row)
        {
            auto listenInterface = std::make_shared<ListenInterface>();
            listenInterface->m_id         = row.GetInt32(0);
            listenInterface->m_host       = row.GetStdString(1);
            listenInterface->m_port       = row.GetInt32(2);
            listenInterface->m_isLocal    = row.GetBool(3);
            listenInterface->m_isOutgoing = row.GetBool(4);
            listenInterface->m_isSsl      = row.GetBool(5);
            result.push_back(listenInterface);
        });

    return result;
}
