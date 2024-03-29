#pragma once

#include <sqlite3.h>

#include "../httprequesthandler.hpp"
#include "../../rpc/command.hpp"
#include "../../sessionmanager.hpp"

namespace pt::Server::Http::Handlers
{
    class JsonRpcHandler : public HttpRequestHandler
    {
    public:
        explicit JsonRpcHandler(sqlite3* db, const std::shared_ptr<pt::Server::SessionManager>& sm);

        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

    private:
        std::map<std::string, std::shared_ptr<pt::Server::RPC::Command>> m_commands;
    };
}
