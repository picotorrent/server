#pragma once

#include <sqlite3.h>

#include "command.hpp"

namespace pt::Server::RPC
{
    class ProxyGetAllCommand : public Command
    {
    public:
        ProxyGetAllCommand(sqlite3* db);
        nlohmann::json Execute(nlohmann::json&) override;

    private:
        sqlite3* m_db;
    };
}
