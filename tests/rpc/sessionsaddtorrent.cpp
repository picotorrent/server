#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../helpers.hpp"
#include "../mocks.hpp"
#include "../../src/json/infohash.hpp"
#include "../../src/rpc/sessionsaddtorrent.hpp"

using pt::Server::RPC::SessionsAddTorrentCommand;

class SessionsAddTorrentCommandTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        sessions = std::make_shared<MockSessionManager>();
        cmd = std::make_unique<SessionsAddTorrentCommand>(sessions);
    }

    std::unique_ptr<SessionsAddTorrentCommand> cmd;
    std::shared_ptr<MockSessionManager> sessions;
    lt::add_torrent_params params;
};

ACTION_P(SaveAddTorrentParams, val, target)
{

}

TEST_F(SessionsAddTorrentCommandTests, Execute_SetsProperties)
{
    auto session = std::make_shared<MockSession>();

    EXPECT_CALL(*sessions, GetDefault())
        .Times(1)
        .WillOnce(::testing::Return(session));

    EXPECT_CALL(*session, AddTorrent(::testing::_))
        .Times(1)
        .WillOnce(
            ::testing::Invoke(
                [&](lt::add_torrent_params& p)
                {
                    EXPECT_EQ(p.save_path, "/tmp");
                    return pt::InfoHashFromString("0101010101010101010101010101010101010101");
                }));

    auto result = cmd->Execute(
        {
            { "save_path", "/tmp" }
        });

    EXPECT_EQ(
        result["result"]["info_hash"],
        "0101010101010101010101010101010101010101");
}
