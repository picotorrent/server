#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pt::Server { class SessionManager; }

namespace pt::Server::RPC
{
    class SessionAddMagnetLinkCommand : public Command
    {
    public:
        SessionAddMagnetLinkCommand(std::shared_ptr<SessionManager>);
        nlohmann::json Execute(nlohmann::json&) override;

    private:
        std::shared_ptr<SessionManager> m_session;
    };
}
