#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pt::Server { class ISessionManager; }

namespace pt::Server::RPC
{
    class SessionAddMagnetLinkCommand : public Command
    {
    public:
        SessionAddMagnetLinkCommand(std::shared_ptr<ISessionManager>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<ISessionManager> m_session;
    };
}
