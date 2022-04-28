#pragma once

#include <memory>
#include <sqlite3.h>

#include "command.hpp"

namespace pt::Server { class Session; }

namespace pt::Server::RPC
{
    class ProxyCreateCommand : public Command
    {
    public:
        explicit ProxyCreateCommand(sqlite3* db);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
    };
}
