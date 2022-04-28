#include "profilesupdate.hpp"

#include <boost/log/trivial.hpp>

#include "../data/datareader.hpp"
#include "../data/models/profile.hpp"
#include "../session.hpp"

using json = nlohmann::json;
using pt::Server::Data::Models::Profile;
using pt::Server::RPC::ProfilesUpdateCommand;
using pt::Server::Session;

ProfilesUpdateCommand::ProfilesUpdateCommand(sqlite3* db, std::shared_ptr<Session> session)
    : m_db(db),
    m_session(session)
{
}

json ProfilesUpdateCommand::Execute(const json& params)
{
    if (!params.is_object())
    {
        return Error(1, "params not an object");
    }

    if (!params.contains("id"))
    {
        return Error(1, "missing 'id' key");
    }

    auto profile = Profile::GetById(m_db, params["id"].get<int>());

    if (profile == nullptr)
    {
        return Error(1, "No profile found with given id");
    }

    if (params.contains("proxy_id"))
    {
        if (params["proxy_id"].is_null())
        {
            profile->ProxyId(std::nullopt);
        }
        else
        {
            profile->ProxyId(params["proxy_id"].get<int>());
        }
    }

    Profile::Update(
        m_db,
        profile);

    if (profile->IsActive())
    {
        m_session->ReloadSettings();
    }

    return Ok(true);
}
