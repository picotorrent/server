#include "transaction.hpp"

#include <boost/log/trivial.hpp>

#include "sqliteexception.hpp"

using pt::Server::Data::Transaction;

Transaction::Transaction(sqlite3* db)
    : m_db(db),
    m_did_act(false)
{
    int res = sqlite3_exec(
        m_db,
        "BEGIN TRANSACTION;",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK)
    {
        throw SQLiteException();
    }
}

Transaction::~Transaction()
{
    if (!m_did_act)
    {
        BOOST_LOG_TRIVIAL(error) << "Transaction left in dangling state";
    }
}

void Transaction::Commit()
{
    int res = sqlite3_exec(
        m_db,
        "COMMIT TRANSACTION;",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK)
    {
        throw SQLiteException();
    }

    m_did_act = true;
}

void Transaction::Rollback()
{
    int res = sqlite3_exec(
        m_db,
        "ROLLBACK TRANSACTION;",
        nullptr,
        nullptr,
        nullptr);

    if (res != SQLITE_OK)
    {
        throw SQLiteException();
    }

    m_did_act = true;
}
