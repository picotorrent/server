#pragma once

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionFindTorrents : public Command
    {
    public:
        explicit SessionFindTorrents(std::weak_ptr<ISession> session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::weak_ptr<ISession> m_session;
    };
}