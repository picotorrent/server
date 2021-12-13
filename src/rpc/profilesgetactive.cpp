#include "profilesgetactive.hpp"

#include <boost/log/trivial.hpp>

#include "../data/models/profile.hpp"
#include "../json/profile.hpp"

using json = nlohmann::json;
using pt::Server::Data::Models::Profile;
using pt::Server::RPC::ProfilesGetActiveCommand;

ProfilesGetActiveCommand::ProfilesGetActiveCommand(sqlite3* db)
    : m_db(db)
{
}

json ProfilesGetActiveCommand::Execute(const json& params)
{
    return Ok(Profile::GetActive(m_db));
}
