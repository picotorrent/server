#pragma once

#include <sqlite3.h>

namespace pika::Data::Migrations
{
    struct InitialSetup
    {
        static int Migrate(sqlite3* db);
    };
}
