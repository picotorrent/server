#include <gtest/gtest.h>
#include <sqlite3.h>

#include "../../src/database.hpp"
#include "../../src/data/models/listeninterface.hpp"

using pt::Server::Database;
using pt::Server::Data::Models::ListenInterface;

class ListenInterfaceTests : public testing::Test
{
protected:
    void SetUp() override
    {
        sqlite3_open(":memory:", &db);
        EXPECT_TRUE(pt::Server::Database::Migrate(db));
    }

    void TearDown() override
    {
        sqlite3_close(db);
    }

    sqlite3* db;
};

TEST_F(ListenInterfaceTests, Create)
{
    ListenInterface::Create(db, "127.0.0.1", 6881, true, true, true);
    auto all = ListenInterface::GetAll(db);

    EXPECT_EQ(all.back()->Host(), "127.0.0.1");
}

TEST_F(ListenInterfaceTests, GetAll)
{
    auto all = ListenInterface::GetAll(db);
    EXPECT_EQ(all[0]->Host(), "0.0.0.0");
    EXPECT_EQ(all[1]->Host(), "[::]");
}
