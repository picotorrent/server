#pragma once

#include <sqlite3.h>

#include "../context.hpp"
#include "../../rpc/command.hpp"
#include "../../session.hpp"

namespace pika::Http::Handlers
{
    class JsonRpcHandler
    {
    public:
        explicit JsonRpcHandler(sqlite3* db, const std::weak_ptr<pika::Session> &session);

        void operator()(const std::shared_ptr<Http::Context>& context);

    private:
        std::map<std::string, std::shared_ptr<pika::RPC::Command>> m_commands;
    };
}
