#include <gtest/gtest.h>
#include <sqlite3.h>

#include "../../src/database.hpp"
#include "../../src/data/statement.hpp"
#include "../../src/rpc/configset.hpp"

using json = nlohmann::json;
using pt::Server::Data::Statement;
using pt::Server::RPC::ConfigSetCommand;

class ConfigSetCommandTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        sqlite3_open(":memory:", &db);
        EXPECT_TRUE(pt::Server::Database::Migrate(db));

        cmd = std::make_unique<ConfigSetCommand>(db);
    }

    void TearDown() override
    {
        sqlite3_close(db);
    }

    json GetValue(const std::string_view& key)
    {
        json v;
        Statement::ForEach(
            db,
            "SELECT value FROM config WHERE key = $1",
            [&v](const auto& row)
            {
                v = json::parse(row.GetStdString(0));
            },
            [&key](auto stmt)
            {
                sqlite3_bind_text(stmt, 1, key.data(), static_cast<int>(key.size()), SQLITE_TRANSIENT);
            });
        return v;
    }

    sqlite3* db;
    std::unique_ptr<ConfigSetCommand> cmd;
};

TEST_F(ConfigSetCommandTests, Execute_WithSingleKey)
{
    EXPECT_EQ(
        cmd->Execute({{ "foo", "bar" }}),
        R"({"result": null})"_json);

    EXPECT_EQ(GetValue("foo"), "bar");
}

TEST_F(ConfigSetCommandTests, Execute_WithMultipleKeys)
{
    EXPECT_EQ(
        cmd->Execute({{ "foo", "bar" }, { "baz", 123 }}),
        R"({"result": null})"_json);

    EXPECT_EQ(GetValue("foo"), "bar");
    EXPECT_EQ(GetValue("baz"), 123);
}
