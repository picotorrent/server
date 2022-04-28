#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pt::Server { class Session; }

namespace pt::Server::RPC
{
    class SessionAddTorrentCommand : public Command
    {
    public:
        explicit SessionAddTorrentCommand(std::shared_ptr<Session>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<Session> m_session;
    };
}
