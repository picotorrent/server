#pragma once

#include <memory>
#include <sqlite3.h>

#include "command.hpp"

namespace pt::Server { class Session; }

namespace pt::Server::RPC
{
    class ListenInterfacesRemoveCommand : public Command
    {
    public:
        explicit ListenInterfacesRemoveCommand(sqlite3* db, std::shared_ptr<Session> session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
        std::shared_ptr<Session> m_session;
    };
}
