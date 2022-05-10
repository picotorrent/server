#pragma once

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionListTorrents : public Command
    {
    public:
        explicit SessionListTorrents(std::shared_ptr<ISession> session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<ISession> m_session;
    };
}