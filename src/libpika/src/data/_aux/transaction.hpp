#pragma once

#include <libpika/data/transaction.hpp>
#include <sqlite3.h>

namespace libpika::data::_aux
{
    class Transaction : public ITransaction
    {
    public:
        explicit Transaction(sqlite3* db);

        void Commit() override;
        void Rollback() override;

    private:
        sqlite3* m_db;
    };
}
