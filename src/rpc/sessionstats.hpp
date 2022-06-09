#pragma once

#include <memory>

#include <libtorrent/session_stats.hpp>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionStatsCommand : public Command
    {
    public:
        explicit SessionStatsCommand(std::weak_ptr<ISession> session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::weak_ptr<ISession> m_session;
        std::vector<libtorrent::stats_metric> m_metrics;
    };
}
