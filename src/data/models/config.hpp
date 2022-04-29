#pragma once

#include <string>

#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace pika::Data::Models
{
    class Config
    {
    public:
        static nlohmann::json Get(sqlite3* db, const std::string_view& key);
        static void Set(sqlite3* db, const std::string_view& key, const nlohmann::json& value);
    };
}
