#include "torrentsfilesget.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/info_hash.hpp>
#include <nlohmann/json.hpp>

#include "../json/infohash.hpp"
#include "../session.hpp"
#include "../torrenthandle.hpp"

namespace lt = libtorrent;

using json = nlohmann::json;
using pika::RPC::TorrentsFilesGetCommand;
using pika::ISession;

TorrentsFilesGetCommand::TorrentsFilesGetCommand(std::weak_ptr<ISession> session)
    : m_session(std::move(session))
{
}

json TorrentsFilesGetCommand::Execute(const json& j)
{
    auto session = m_session.lock();

    if (!session)
    {
        return Error(99, "Failed to lock session");
    }

    if (j.is_array())
    {
        json result;

        for (lt::info_hash_t const& hash : j.get<std::vector<lt::info_hash_t>>())
        {
            json r = {
                {"info_hash", hash}
            };

            if (auto torrent = session->FindTorrent(hash))
            {
                const lt::torrent_status& ts = torrent->Status();

                if (auto tf = ts.torrent_file.lock())
                {
                    const lt::file_storage& fs = tf->files();

                    for (int i = 0; i < fs.num_files(); i++)
                    {
                        lt::file_index_t idx{i};

                        json f = {
                            {"index", static_cast<int32_t>(idx)},
                            {"path", fs.file_path(idx)},
                            {"size", fs.file_size(idx)},
                            {"flags", static_cast<uint8_t>(fs.file_flags(idx))}
                        };

                        r["files"].push_back(f);
                    }
                }
                else
                {
                    r["error"] = "Could not lock torrent_file object";
                }
            }
            else
            {
                r["error"] = "Failed to find torrent";
            }

            result.push_back(r);
        }

        return Ok(result);
    }

    return Error(1, "Params needs to be an array of info hash arrays");
}
