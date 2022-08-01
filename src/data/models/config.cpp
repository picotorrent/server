#include "config.hpp"

#include <libpika/data/database.hpp>
#include <libpika/data/row.hpp>
#include <libpika/data/statement.hpp>

using json = nlohmann::json;
using pika::Data::Models::Config;

json Config::Get(libpika::data::Database& db, const std::string_view &key)
{
    json result = {};

    auto stmt = db.PrepareStatement("SELECT value FROM config WHERE key = $1");
    stmt->Bind(1, key);

    if (auto row = stmt->Step())
    {
        result = json::parse(row->GetStdString(0));
    }

    return result;
}

void Config::Set(libpika::data::Database& db, const std::string_view& key, const json& value)
{
    auto stmt = db.PrepareStatement("REPLACE INTO config (key, value) VALUES ($1, $2)");
    stmt->Bind(1, key);
    stmt->Bind(2, value.dump());
    stmt->Step();
}