#pragma once

#include "command.hpp"

#include <sqlite3.h>

namespace pika::RPC
{
    class ConfigSetCommand : public Command
    {
    public:
        ConfigSetCommand(sqlite3* db);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
    };
}
