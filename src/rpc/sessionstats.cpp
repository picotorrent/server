#include "sessionstats.hpp"

#include "../session.hpp"

using json = nlohmann::json;
using pika::RPC::SessionStatsCommand;

SessionStatsCommand::SessionStatsCommand(ISession& session)
        : m_session(session)
{
}

json SessionStatsCommand::Execute(const json &req)
{
    return Ok(m_session.Counters());
}
