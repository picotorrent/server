#include "torrentsaddlabels.hpp"

using pika::RPC::TorrentsAddLabelsCommand;

TorrentsAddLabelsCommand::TorrentsAddLabelsCommand(sqlite3 *db, std::weak_ptr<ISession> session)
    : m_db(db)
    , m_session(std::move(session))
{
}

nlohmann::json TorrentsAddLabelsCommand::Execute(const nlohmann::json& req)
{
    return Error(1, "Invalid request");
}
