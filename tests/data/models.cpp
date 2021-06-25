#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <catch2/catch.hpp>
#include <sqlite3.h>

#include "../../src/database.hpp"
#include "../../src/data/models/listeninterface.hpp"

using pt::Server::Database;
using pt::Server::Data::Models::ListenInterface;

void DisableLoggingOutput()
{
    boost::log::core::get()->set_filter(boost::log::trivial::severity > boost::log::trivial::fatal);
}

TEST_CASE("ListenInterface")
{
    DisableLoggingOutput();

    sqlite3* db;
    sqlite3_open(":memory:", &db);

    REQUIRE(pt::Server::Database::Migrate(db));

    SECTION("Create")
    {
        ListenInterface::Create(db, "127.0.0.1", 6881, true, true, true);
        auto all = ListenInterface::GetAll(db);

        REQUIRE(all.back()->Host() == "127.0.0.1");
    }

    SECTION("GetAll")
    {
        auto all = ListenInterface::GetAll(db);
        REQUIRE(all[0]->Host() == "0.0.0.0");
        REQUIRE(all[1]->Host() == "[::]");
    }

    sqlite3_close(db);
}
