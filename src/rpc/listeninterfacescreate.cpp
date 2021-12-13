#include "listeninterfacescreate.hpp"

#include <boost/log/trivial.hpp>

#include "../data/datareader.hpp"
#include "../data/models/listeninterface.hpp"
#include "../sessionmanager.hpp"

using json = nlohmann::json;
using pt::Server::Data::Models::ListenInterface;
using pt::Server::RPC::ListenInterfacesCreateCommand;
using pt::Server::SessionManager;

ListenInterfacesCreateCommand::ListenInterfacesCreateCommand(sqlite3* db, std::shared_ptr<SessionManager> session)
    : m_db(db),
    m_session(session)
{
}

json ListenInterfacesCreateCommand::Execute(const json& params)
{
    if (params.is_object())
    {
        auto listenInterface = ListenInterface::Create(
            m_db,
            params["host"].get<std::string>(),
            params["port"].get<int>(),
            params["is_local"].get<bool>(),
            params["is_outgoing"].get<bool>(),
            params["is_ssl"].get<bool>());

        // Reload session settings to reload listen interfaces
        m_session->ReloadSettings();

        return Ok({ {"id", listenInterface->Id() } });
    }

    return Error(1, "params not an object");
}
