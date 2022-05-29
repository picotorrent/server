#include "torrentssetlabels.hpp"

#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>

#include "../data/models/labels.hpp"
#include "../json/infohash.hpp"
#include "../session.hpp"
#include "../torrenthandle.hpp"

namespace lt = libtorrent;

using json = nlohmann::json;
using pika::Data::Models::Labels;
using pika::RPC::TorrentsSetLabelsCommand;
using pika::ISession;

TorrentsSetLabelsCommand::TorrentsSetLabelsCommand(sqlite3* db, std::weak_ptr<ISession> session)
    : m_db(db)
    , m_session(std::move(session))
{
}

json TorrentsSetLabelsCommand::Execute(const json& j)
{
    auto session = m_session.lock();

    if (!session)
    {
        return Error(99, "Failed to lock session");
    }

    if (j.is_array())
    {
        for (const auto& item : j)
        {
            lt::info_hash_t hash = item["info_hash"].get<lt::info_hash_t>();
            auto labels = item["labels"].get<std::map<std::string, std::string>>();

            Labels::Delete(m_db, hash);
            Labels::Set(m_db, hash, labels);
        }

        return Ok(true);
    }

    return Error(1, "'params' not a string or array of strings");
}
