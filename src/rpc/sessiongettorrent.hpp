#pragma once

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionGetTorrentCommand : public Command
    {
    public:
        explicit SessionGetTorrentCommand(std::shared_ptr<ISession> session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<ISession> m_session;
    };
}