#include <gtest/gtest.h>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session_params.hpp>
#include <sqlite3.h>

#include "../helpers.hpp"
#include "../../src/data/migrator.hpp"
#include "../../src/data/models/addtorrentparams.hpp"
#include "../../src/data/models/sessionparams.hpp"

namespace lt = libtorrent;
using pika::Data::Migrator;
using pika::Data::Models::AddTorrentParams;
using pika::Data::Models::SessionParams;

class ModelsTests : public testing::Test
{
protected:
    void SetUp() override
    {
        sqlite3_open(":memory:", &db);
        EXPECT_TRUE(pika::Data::Migrator::Run(db));
    }

    void TearDown() override
    {
        sqlite3_close(db);
    }

    sqlite3* db{};
};

TEST_F(ModelsTests, AddTorrentParams_Insert)
{
    lt::add_torrent_params p;
    p.info_hashes = pt::InfoHashFromString("0101010101010101010101010101010101010101");
    p.save_path = "/tmp";

    AddTorrentParams::Insert(db, p, 1);

    EXPECT_EQ(AddTorrentParams::Count(db), 1);
}

TEST_F(ModelsTests, SessionParams_Insert)
{
    lt::session_params sp;
    SessionParams::Insert(db, sp);
}
