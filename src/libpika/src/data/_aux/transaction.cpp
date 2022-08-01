#include "transaction.hpp"

using libpika::data::_aux::Transaction;

Transaction::Transaction(sqlite3 *db)
    : m_db(db)
{
}

void Transaction::Commit()
{
}

void Transaction::Rollback()
{
}
