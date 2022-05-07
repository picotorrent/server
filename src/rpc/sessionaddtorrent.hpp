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
        explicit SessionAddTorrentCommand(std::shared_ptr<ISession>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<ISession> m_session;
    };
}
