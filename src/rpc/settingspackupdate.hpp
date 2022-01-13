#pragma once

#include "command.hpp"

#include <memory>
#include <sqlite3.h>

namespace pt::Server { class ISessionManager; }

namespace pt::Server::RPC
{
    class SettingsPackUpdateCommand : public Command
    {
    public:
        SettingsPackUpdateCommand(sqlite3* db, std::shared_ptr<pt::Server::ISessionManager>  session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
        std::shared_ptr<pt::Server::ISessionManager> m_session;
    };
}
