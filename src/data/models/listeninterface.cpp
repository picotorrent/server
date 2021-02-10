#include "listeninterface.hpp"

#include "../datareader.hpp"

using pt::Server::Data::Models::ListenInterface;

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
