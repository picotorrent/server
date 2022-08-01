#pragma once

#include <functional>
#include <vector>

namespace libpika::data
{
    class Database;

    class Migrator
    {
    public:
        static bool Run(Database& db, const std::vector<std::function<void(Database&)>>& migrations);
    };
}
