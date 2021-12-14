#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sqlite3.h>

#include "../helpers.hpp"
#include "../mocks.hpp"
#include "../../src/json/infohash.hpp"
#include "../../src/rpc/torrentsresume.hpp"

using pt::Server::RPC::TorrentsResumeCommand;

class TorrentsResumeCommandTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        finder = std::make_shared<MockTorrentHandleFinder>();
        cmd = std::make_unique<TorrentsResumeCommand>(finder);
    }

    std::unique_ptr<TorrentsResumeCommand> cmd;
    std::shared_ptr<MockTorrentHandleFinder> finder;
};

TEST_F(TorrentsResumeCommandTests, Execute_WithInvalidParams_ReturnsError)
{
    auto result = cmd->Execute(1);

    EXPECT_TRUE(result.contains("error"));
    EXPECT_EQ(result["error"]["code"], 1);
}

TEST_F(TorrentsResumeCommandTests, Execute_WithValidInfoHash_ResumesTorrent)
{
    auto hash = pt::InfoHashFromString("7cf55428325617fdde910fe55b79ab72be937924");
    auto handle = std::make_shared<MockTorrentHandleActor>();

    EXPECT_CALL(*finder, Find(hash))
        .Times(1)
        .WillOnce(::testing::Return(handle));

    EXPECT_CALL(*handle, IsValid())
        .Times(1)
        .WillOnce(::testing::Return(true));

    EXPECT_CALL(*handle, Resume())
        .Times(1);

    auto result = cmd->Execute("7cf55428325617fdde910fe55b79ab72be937924");

    EXPECT_TRUE(result.is_object());
}

TEST_F(TorrentsResumeCommandTests, Execute_WithValidInfoHashArray_ResumesTorrents)
{
    struct F
    {
        lt::info_hash_t ih;
        std::shared_ptr<MockTorrentHandleActor> handle;
    };

    std::vector<F> items;
    items.push_back({ pt::InfoHashFromString("7cf55428325617fdde910fe55b79ab72be937924"), std::make_shared<MockTorrentHandleActor>() });
    items.push_back({ pt::InfoHashFromString("0202020202020202020202020202020202020202"), std::make_shared<MockTorrentHandleActor>() });
    items.push_back({ pt::InfoHashFromString("0303030303030303030303030303030303030303"), std::make_shared<MockTorrentHandleActor>() });

    auto handle = std::make_shared<MockTorrentHandleActor>();

    for (auto const& itm : items)
    {
        EXPECT_CALL(*finder, Find(itm.ih))
                .Times(1)
                .WillOnce(::testing::Return(itm.handle));

        EXPECT_CALL(*itm.handle, IsValid())
                .Times(1)
                .WillOnce(::testing::Return(true));

        EXPECT_CALL(*itm.handle, Resume())
                .Times(1);
    }

    auto result = cmd->Execute(
        { "7cf55428325617fdde910fe55b79ab72be937924",
          "0202020202020202020202020202020202020202",
          "0303030303030303030303030303030303030303" });

    EXPECT_TRUE(result.is_object());
}
