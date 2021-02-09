#pragma once

#include "command.hpp"

#include <sqlite3.h>

namespace pt::Server::RPC
{
    class ListenInterfacesGetAllCommand : public Command
    {
    public:
        ListenInterfacesGetAllCommand(sqlite3* db);
        nlohmann::json Execute(nlohmann::json&) override;

    private:
        sqlite3* m_db;
    };
}
