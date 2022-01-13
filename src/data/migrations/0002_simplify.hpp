#pragma once

#include <sqlite3.h>

namespace pt::Server::Data::Migrations
{
    struct Simplify
    {
        static int Migrate(sqlite3* db);
    };
}
