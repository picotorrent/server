#pragma once

#include <sqlite3.h>

#include "../httprequesthandler.hpp"
#include "../../rpc/command.hpp"
#include "../../session.hpp"

namespace pika::Http::Handlers
{
    class JsonRpcHandler : public HttpRequestHandler
    {
    public:
        explicit JsonRpcHandler(sqlite3* db, const std::shared_ptr<pika::Session> &session);

        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

    private:
        std::map<std::string, std::shared_ptr<pika::RPC::Command>> m_commands;
    };
}
