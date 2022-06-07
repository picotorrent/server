#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsMoveStorageCommand : public Command
    {
    public:
        explicit TorrentsMoveStorageCommand(std::weak_ptr<ISession>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::weak_ptr<ISession> m_session;
    };
}