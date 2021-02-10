#pragma once

#pragma once

#include <sqlite3.h>

namespace pt::Server::Data::Migrations
{
    struct InitialSetup
    {
        static int Migrate(sqlite3* db);
    };
}
