#include "sessionstats.hpp"

#include <libpika/bittorrent/session.hpp>

using json = nlohmann::json;
using pika::RPC::SessionStatsCommand;

SessionStatsCommand::SessionStatsCommand(libpika::bittorrent::ISession& session)
        : m_session(session)
{
}

json SessionStatsCommand::Execute(const json &req)
{
    return Ok(m_session.Counters());
}
