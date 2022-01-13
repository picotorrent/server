#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pt::Server { class ISessionManager; }

namespace pt::Server::RPC
{
    class TorrentsPauseCommand : public Command
    {
    public:
        TorrentsPauseCommand(std::shared_ptr<ISessionManager>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<ISessionManager> m_sessions;
    };
}
