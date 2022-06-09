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
        return Ok(session->Counters());
    }

    return Error(99, "Failed to lock session");
}
