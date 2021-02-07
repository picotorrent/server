#pragma once

#include <sqlite3.h>

namespace pt::Server
{
    class Database
    {
    public:
        static bool Migrate(sqlite3* db);
    };
}
