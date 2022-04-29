#pragma once

#include <sqlite3.h>

namespace pika::Data
{
    class Migrator
    {
    public:
        static bool Run(sqlite3* db);
    };
}
