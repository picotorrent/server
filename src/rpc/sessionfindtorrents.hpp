#pragma once

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionFindTorrents : public Command
    {
    public:
        explicit SessionFindTorrents(ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        ISession& m_session;
    };
}