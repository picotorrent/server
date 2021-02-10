#pragma once

#include <memory>
#include <sqlite3.h>

#include "command.hpp"

namespace pt::Server { class SessionManager; }

namespace pt::Server::RPC
{
    class ProxyCreateCommand : public Command
    {
    public:
        ProxyCreateCommand(sqlite3* db, std::shared_ptr<SessionManager> session);
        nlohmann::json Execute(nlohmann::json&) override;

    private:
        sqlite3* m_db;
        std::shared_ptr<SessionManager> m_session;
    };
}
