#include <gtest/gtest.h>

#include <map>
#include <memory>

#include <libpika/jsonrpc/jsonrpcserver.hpp>
#include <libpika/jsonrpc/method.hpp>

class TestMethod : public libpika::jsonrpc::Method
{
public:
    nlohmann::json Execute(const nlohmann::json& params) override
    {
        return "OK!";
    }
};

class JsonRpcServerTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        server = std::make_unique<libpika::jsonrpc::JsonRpcServer>(
            std::map<std::string, std::shared_ptr<libpika::jsonrpc::Method>>({
                {"test1", std::make_shared<TestMethod>()}
            }));
    }

    std::unique_ptr<libpika::jsonrpc::JsonRpcServer> server;
};

TEST_F(JsonRpcServerTests, Execute_WithInvalidRequest)
{
    auto result = server->Execute(
        {
        });

    EXPECT_EQ(result["error"]["code"], -32600);
    EXPECT_EQ(result["jsonrpc"],       "2.0");
}

TEST_F(JsonRpcServerTests, Execute_WithMissingMethod)
{
    auto result = server->Execute(
        {
            {"jsonrpc", "2.0"},
            {"method", "missing"}
        });

    EXPECT_EQ(result["error"]["code"], -32601);
    EXPECT_EQ(result["jsonrpc"],       "2.0");
}

TEST_F(JsonRpcServerTests, Execute_WithRealMethod)
{
    auto result = server->Execute(
        {
            {"jsonrpc", "2.0"},
            {"method", "test1"}
        });

    EXPECT_EQ(result["jsonrpc"], "2.0");
    EXPECT_EQ(result["result"],  "OK!");
}
