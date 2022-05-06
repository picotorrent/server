#include "config.hpp"

#include "../statement.hpp"

using json = nlohmann::json;
using pika::Data::Models::Config;
using pika::Data::Statement;

json Config::Get(sqlite3* db, const std::string_view &key)
{
    json result = {};

    Statement::ForEach(
        db,
        "SELECT value FROM config WHERE key = $1",
        [&result](const auto& row)
        {
            result = json::parse(row.GetStdString(0));
        },
        [&key](auto stmt)
        {
            return sqlite3_bind_text(stmt, 1, key.data(), static_cast<int>(key.size()), SQLITE_TRANSIENT);
        });

    return result;
}

void Config::Set(sqlite3* db, const std::string_view& key, const json& value)
{
    Statement::ForEach(
        db,
        "REPLACE INTO config (key, value) VALUES ($1, $2)",
        [](const auto &)
        {},
        [&key, &value](auto stmt)
        {
            CHECK_OK(sqlite3_bind_text(stmt, 1, key.data(),           -1, SQLITE_TRANSIENT))
            CHECK_OK(sqlite3_bind_text(stmt, 2, value.dump().c_str(), -1, SQLITE_TRANSIENT))
            return SQLITE_OK;
        });
}