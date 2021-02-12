#pragma once

#include <memory>
#include <sqlite3.h>

#include "command.hpp"

namespace pt::Server { class SessionManager; }

namespace pt::Server::RPC
{
    class ProfilesUpdateCommand : public Command
    {
    public:
        ProfilesUpdateCommand(sqlite3* db, std::shared_ptr<SessionManager> session);
        nlohmann::json Execute(nlohmann::json&) override;

    private:
        sqlite3* m_db;
        std::shared_ptr<SessionManager> m_session;
    };
}
