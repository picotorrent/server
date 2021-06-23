#include "listeninterfacesgetall.hpp"

#include "../data/models/listeninterface.hpp"
#include "../json/listeninterface.hpp"

using json = nlohmann::json;
using pt::Server::Data::Models::ListenInterface;
using pt::Server::RPC::ListenInterfacesGetAllCommand;

ListenInterfacesGetAllCommand::ListenInterfacesGetAllCommand(sqlite3* db)
    : m_db(db)
{
}

json ListenInterfacesGetAllCommand::Execute(json& params)
{
    return Ok(ListenInterface::GetAll(m_db));
}
