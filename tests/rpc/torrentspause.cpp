#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../helpers.hpp"
#include "../mocks.hpp"
#include "../../src/json/infohash.hpp"
#include "../../src/rpc/torrentspause.hpp"

using pt::Server::RPC::TorrentsPauseCommand;

class TorrentsPauseCommandTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        sessions = std::make_shared<MockSessionManager>();
        cmd = std::make_unique<TorrentsPauseCommand>(sessions);
    }

    std::unique_ptr<TorrentsPauseCommand> cmd;
    std::shared_ptr<MockSessionManager> sessions;
};

TEST_F(TorrentsPauseCommandTests, Execute_WithInvalidParams_ReturnsError)
{
    EXPECT_CALL(*sessions, GetDefault())
        .Times(0);

    auto result = cmd->Execute(1);

    EXPECT_TRUE(result.contains("error"));
    EXPECT_EQ(result["error"]["code"], 1);
}

TEST_F(TorrentsPauseCommandTests, Execute_WithValidInfoHash_PausesTorrent)
{
    lt::info_hash_t ih = pt::InfoHashFromString("0101010101010101010101010101010101010101");

    auto session = std::make_shared<MockSession>();
    auto handle = std::make_shared<MockTorrentHandle>();

    EXPECT_CALL(*sessions, GetDefault())
        .Times(1)
        .WillOnce(::testing::Return(session));

    EXPECT_CALL(*session, FindTorrent(ih))
        .Times(1)
        .WillOnce(::testing::Return(handle));

    EXPECT_CALL(*handle, IsValid())
        .Times(1)
        .WillOnce(::testing::Return(true));

    EXPECT_CALL(*handle, Pause())
        .Times(1);

    auto result = cmd->Execute("0101010101010101010101010101010101010101");

    EXPECT_TRUE(result.is_object());
}

TEST_F(TorrentsPauseCommandTests, Execute_WithValidInfoHashArray_PausesTorrents)
{
    struct F
    {
        lt::info_hash_t ih;
        std::shared_ptr<MockTorrentHandle> handle;
    };

    std::vector<F> items;
    items.push_back({ pt::InfoHashFromString("0101010101010101010101010101010101010101"), std::make_shared<MockTorrentHandle>() });
    items.push_back({ pt::InfoHashFromString("0202020202020202020202020202020202020202"), std::make_shared<MockTorrentHandle>() });
    items.push_back({ pt::InfoHashFromString("0303030303030303030303030303030303030303"), std::make_shared<MockTorrentHandle>() });

    auto session = std::make_shared<MockSession>();

    EXPECT_CALL(*sessions, GetDefault())
        .Times(1)
        .WillOnce(::testing::Return(session));

    for (auto const& itm : items)
    {
        EXPECT_CALL(*session, FindTorrent(itm.ih))
                .Times(1)
                .WillOnce(::testing::Return(itm.handle));

        EXPECT_CALL(*itm.handle, IsValid())
                .Times(1)
                .WillOnce(::testing::Return(true));

        EXPECT_CALL(*itm.handle, Pause())
                .Times(1);
    }

    auto result = cmd->Execute(
        { "0101010101010101010101010101010101010101",
          "0202020202020202020202020202020202020202",
          "0303030303030303030303030303030303030303" });

    EXPECT_TRUE(result.is_object());
}
