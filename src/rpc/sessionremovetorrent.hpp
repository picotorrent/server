#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pt::Server { class Session; }

namespace pt::Server::RPC
{
    class SessionRemoveTorrentCommand : public Command
    {
    public:
        explicit SessionRemoveTorrentCommand(std::shared_ptr<Session>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<Session> m_session;
    };
}
