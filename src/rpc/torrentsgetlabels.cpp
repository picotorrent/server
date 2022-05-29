#include "torrentsgetlabels.hpp"

#include <libtorrent/info_hash.hpp>

#include "../data/models/labels.hpp"
#include "../json/infohash.hpp"
#include "../session.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::Data::Models::Labels;
using pika::RPC::TorrentsGetLabelsCommand;

TorrentsGetLabelsCommand::TorrentsGetLabelsCommand(sqlite3 *db, std::weak_ptr<ISession> session)
    : m_db(db)
    , m_session(std::move(session))
{
}

json TorrentsGetLabelsCommand::Execute(const json& req)
{
    auto session = m_session.lock();

    if (!session)
    {
        return Error(99, "Could not lock session");
    }

    if (req.is_array())
    {
        json::array_t result;

        for (const auto& item : req)
        {
            lt::info_hash_t hash = item.get<lt::info_hash_t>();

            if (auto torrent = session->FindTorrent(hash))
            {
                result.push_back({
                    {"info_hash", hash},
                    {"labels", Labels::Get(m_db, hash)}
                });
            }
        }

        return Ok(result);
    }

    return Error(1, "Invalid request");
}
