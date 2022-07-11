#pragma once

#include <libpika/http/context.hpp>
#include <sqlite3.h>

#include "../../rpc/command.hpp"

namespace pika
{
    class ISession;
}

namespace pika::Http::Handlers
{
    class JsonRpcHandler
    {
    public:
        explicit JsonRpcHandler(std::map<std::string, std::shared_ptr<pika::RPC::Command>>  cmds);

        void operator()(const std::shared_ptr<libpika::http::Context>& context);

    private:
        std::map<std::string, std::shared_ptr<pika::RPC::Command>> m_commands;
    };
}
