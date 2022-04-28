#pragma once

#include <sqlite3.h>

namespace pt::Server::Data
{
    class Migrator
    {
    public:
        static bool Run(sqlite3* db);
    };
}
