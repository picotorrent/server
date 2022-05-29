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
        explicit SessionAddTorrentCommand(std::weak_ptr<ISession> session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::weak_ptr<ISession> m_session;
    };
}
