#include "proxygetall.hpp"

#include "../data/models/proxy.hpp"
#include "../json/proxy.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::Models::Proxy;
using pt::Server::RPC::ProxyGetAllCommand;

ProxyGetAllCommand::ProxyGetAllCommand(sqlite3* db)
    : m_db(db)
{
}

json ProxyGetAllCommand::Execute(json& params)
{
    return Ok(Proxy::GetAll(m_db));
}
