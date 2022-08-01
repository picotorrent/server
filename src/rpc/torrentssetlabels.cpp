#include "torrentssetlabels.hpp"

#include <boost/log/trivial.hpp>
#include <libpika/bittorrent/session.hpp>
#include <nlohmann/json.hpp>

#include "../json/infohash.hpp"

namespace lt = libtorrent;

using json = nlohmann::json;
using pika::RPC::TorrentsLabelsSetCommand;

TorrentsLabelsSetCommand::TorrentsLabelsSetCommand(libpika::bittorrent::ISession& session)
    : m_session(session)
{
}

json TorrentsLabelsSetCommand::Execute(const json& j)
{
    if (j.is_array())
    {
        for (const auto& item : j)
        {
            lt::info_hash_t hash = item["info_hash"].get<lt::info_hash_t>();
            auto labels = item["labels"].get<std::map<std::string, std::string>>();

            /*Labels::Delete(m_db, hash);
            Labels::Set(m_db, hash, labels);*/
        }

        return Ok(true);
    }

    return Error(1, "'params' not a string or array of strings");
}
