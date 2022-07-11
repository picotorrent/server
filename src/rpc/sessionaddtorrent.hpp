#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionAddTorrentCommand : public Command
    {
    public:
        explicit SessionAddTorrentCommand(ISession& session);

        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        ISession& m_session;
    };
}
