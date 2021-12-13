#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sqlite3.h>

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

TEST_F(TorrentsResumeCommandTests, Execute_WithValidInfoHash_PausesTorrent)
{
    lt::info_hash_t ih(lt::sha1_hash("01010101010101010101"));

    auto handle = std::make_shared<MockTorrentHandleActor>();

    EXPECT_CALL(*finder, Find(ih))
        .Times(1)
        .WillOnce(::testing::Return(handle));

    EXPECT_CALL(*handle, IsValid())
        .Times(1)
        .WillOnce(::testing::Return(true));

    EXPECT_CALL(*handle, Resume())
        .Times(1);

    auto result = cmd->Execute(ih);

    EXPECT_TRUE(result.is_object());
}

TEST_F(TorrentsResumeCommandTests, Execute_WithValidInfoHashArray_PausesTorrents)
{
    struct F
    {
        lt::info_hash_t ih;
        std::shared_ptr<MockTorrentHandleActor> handle;
    };

    std::vector<F> items;
    items.push_back({ lt::info_hash_t(lt::sha1_hash("01010101010101010101")), std::make_shared<MockTorrentHandleActor>() });
    items.push_back({ lt::info_hash_t(lt::sha1_hash("02020202020202020202")), std::make_shared<MockTorrentHandleActor>() });
    items.push_back({ lt::info_hash_t(lt::sha1_hash("03030303030303030303")), std::make_shared<MockTorrentHandleActor>() });

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
    {
        lt::info_hash_t(lt::sha1_hash("01010101010101010101")),
        lt::info_hash_t(lt::sha1_hash("02020202020202020202")),
        lt::info_hash_t(lt::sha1_hash("03030303030303030303"))
    });

    EXPECT_TRUE(result.is_object());
}
