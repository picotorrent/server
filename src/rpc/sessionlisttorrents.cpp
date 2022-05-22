#include "sessionlisttorrents.hpp"

#include <utility>

#include "../json/torrentstatus.hpp"
#include "../session.hpp"

using json = nlohmann::json;
using pika::RPC::SessionListTorrents;

SessionListTorrents::SessionListTorrents(std::shared_ptr<ISession> session)
    : m_session(std::move(session))
{
}

json SessionListTorrents::Execute(const json &req)
{
    json::array_t j;

    m_session->ForEachTorrent(
        [&j](const lt::torrent_status &status)
        {
            j.push_back(status);
        });

    return Ok(j);
}
