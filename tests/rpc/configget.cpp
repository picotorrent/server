#include <gtest/gtest.h>
#include <sqlite3.h>

#include "../../src/data/migrator.hpp"
#include "../../src/data/statement.hpp"
#include "../../src/rpc/configget.hpp"

using json = nlohmann::json;
using pt::Server::Data::Statement;
using pt::Server::RPC::ConfigGetCommand;

class ConfigGetCommandTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        sqlite3_open(":memory:", &db);
        EXPECT_TRUE(pt::Server::Data::Migrator::Run(db));

        cmd = std::make_unique<ConfigGetCommand>(db);
    }

    void TearDown() override
    {
        sqlite3_close(db);
    }

    void InsertValue(const std::string_view& key, const json& value)
    {
        Statement::ForEach(
            db,
            "REPLACE INTO config (key, value) VALUES ($1, $2)",
            [](auto const&){},
            [&key, &value](auto stmt)
            {
                sqlite3_bind_text(stmt, 1, key.data(),          -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 2, value.dump().c_str(), -1, SQLITE_TRANSIENT);
            });
    }

    json ToResult(const json& value)
    {
        return {
            { "result", value }
        };
    }

    sqlite3* db;
    std::unique_ptr<ConfigGetCommand> cmd;
};

TEST_F(ConfigGetCommandTests, Execute_WithNonExistingStringKey_ReturnsNullResult)
{
    EXPECT_EQ(cmd->Execute("foo"), ToResult({}));
}

TEST_F(ConfigGetCommandTests, Execute_WithExistingKey_ReturnsValue)
{
    InsertValue("foo", R"([ 1, 2, 3 ])"_json);
    EXPECT_EQ(cmd->Execute("foo"), ToResult({ 1, 2, 3 }));
}

TEST_F(ConfigGetCommandTests, Execute_WithMultipleExistingKeys_ReturnsValues)
{
    InsertValue("foo", R"([ 1, 2, 3 ])"_json);
    InsertValue("bar", R"({ "baz": 1 })"_json);

    EXPECT_EQ(
        cmd->Execute({ "foo", "bar" }),
        ToResult(R"({ "foo": [1,2,3], "bar":{"baz":1}})"_json));
}

TEST_F(ConfigGetCommandTests, Execute_WithOneExistingAndOneMissingKey_ReturnsMixedResult)
{
    InsertValue("foo", R"([ 1, 2, 3 ])"_json);

    EXPECT_EQ(
            cmd->Execute({ "foo", "bar" }),
            ToResult(R"({ "foo": [1,2,3], "bar": null })"_json));
}
