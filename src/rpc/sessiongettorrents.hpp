#pragma once

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionGetTorrentsCommand : public Command
    {
    public:
        explicit SessionGetTorrentsCommand(std::weak_ptr<ISession> session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::weak_ptr<ISession> m_session;
    };
}