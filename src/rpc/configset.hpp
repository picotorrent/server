#pragma once

#include "command.hpp"

#include <sqlite3.h>

namespace pt::Server::RPC
{
    class ConfigSetCommand : public Command
    {
    public:
        ConfigSetCommand(sqlite3* db);
        nlohmann::json Execute(nlohmann::json&) override;

    private:
        sqlite3* m_db;
    };
}
