#pragma once

#include <string>

#include <nlohmann/json.hpp>

namespace libpika::data
{
    class Database;
}

namespace pika::Data::Models
{
    class Config
    {
    public:
        static nlohmann::json Get(libpika::data::Database& db, const std::string_view& key);
        static void Set(libpika::data::Database& db, const std::string_view& key, const nlohmann::json& value);
    };
}
