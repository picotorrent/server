#include "profilesgetall.hpp"

#include <boost/log/trivial.hpp>

#include "../data/datareader.hpp"
#include "../data/models/profile.hpp"
#include "../json/profile.hpp"

using json = nlohmann::json;
using pt::Server::Data::Models::Profile;
using pt::Server::RPC::ProfilesGetAllCommand;

ProfilesGetAllCommand::ProfilesGetAllCommand(sqlite3* db)
    : m_db(db)
{
}

json ProfilesGetAllCommand::Execute(json& params)
{
    return Ok(Profile::GetAll(m_db));
}
