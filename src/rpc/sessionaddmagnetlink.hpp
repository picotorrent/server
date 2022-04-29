#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pika { class Session; }

namespace pika::RPC
{
    class SessionAddMagnetLinkCommand : public Command
    {
    public:
        explicit SessionAddMagnetLinkCommand(std::shared_ptr<Session>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<Session> m_session;
    };
}
