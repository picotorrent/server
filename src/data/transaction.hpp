#pragma once

#include <sqlite3.h>

namespace pika::Data
{
    struct Transaction
    {
        Transaction(sqlite3* db);
        ~Transaction();

        void Commit();
        void Rollback();

    private:
        sqlite3* m_db;
        bool m_did_act;
    };
}
