#include "sessionstats.hpp"

#include <utility>

#include <libtorrent/session_stats.hpp>

#include "../session.hpp"

using json = nlohmann::json;
using pika::RPC::SessionStatsCommand;

SessionStatsCommand::SessionStatsCommand(std::weak_ptr<ISession> session)
        : m_session(std::move(session))
        , m_metrics(lt::session_stats_metrics())
{
}

json SessionStatsCommand::Execute(const json &req)
{
    if (auto session = m_session.lock())
    {
        const auto& counters = session->Counters();
        std::map<std::string, int64_t> stats;

        for (const auto& sm : m_metrics)
        {
            stats[sm.name] = counters[sm.value_index];
        }

        return Ok(stats);
    }

    return Error(99, "Failed to lock session");
}
