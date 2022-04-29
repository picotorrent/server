#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsResumeCommand : public Command
    {
    public:
        TorrentsResumeCommand(std::shared_ptr<ISession>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<ISession> m_session;
    };
}
