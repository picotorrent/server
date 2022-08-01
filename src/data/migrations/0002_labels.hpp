#pragma once

#include <sqlite3.h>

namespace pika::Data::Migrations
{
    struct Labels
    {
        static int Migrate(sqlite3* db);
    };
}
