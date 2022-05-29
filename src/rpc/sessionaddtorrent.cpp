#include "sessionaddtorrent.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/bdecode.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../json/infohash.hpp"
#include "../session.hpp"

static std::string Base64Decode(const std::string_view in)
{
    // table from '+' to 'z'
    const uint8_t lookup[] = {
        62,  255, 62,  255, 63,  52,  53, 54, 55, 56, 57, 58, 59, 60, 61, 255,
        255, 0,   255, 255, 255, 255, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
        10,  11,  12,  13,  14,  15,  16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        255, 255, 255, 255, 63,  255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36,  37,  38,  39,  40,  41,  42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };
    static_assert(sizeof(lookup) == 'z' - '+' + 1);

    std::string out;
    int val = 0, valb = -8;
    for (uint8_t c : in) {
        if (c < '+' || c > 'z')
            break;
        c -= '+';
        if (lookup[c] >= 64)
            break;
        val = (val << 6) + lookup[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

namespace lt = libtorrent;
using json = nlohmann::json;
using pika::Session;
using pika::RPC::SessionAddTorrentCommand;

SessionAddTorrentCommand::SessionAddTorrentCommand(std::weak_ptr<ISession> session)
    : m_session(std::move(session))
{
}

json SessionAddTorrentCommand::Execute(const json& j)
{
    if (!j.contains("ti") && !j.contains("magnet_uri"))
    {
        return Error(1, "Missing 'ti' or 'magnet_uri' field");
    }

    if (!j.contains("save_path"))
    {
        return Error(1, "Missing 'save_path' field");
    }

    lt::add_torrent_params p;

    if (j.contains("magnet_uri"))
    {
        lt::error_code ec;
        lt::parse_magnet_uri(
            j["magnet_uri"].get<std::string>(),
                p,
                ec);

        if (ec)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to parse magnet URI: " << ec.message();
            return Error(1, ec.message());
        }
    }
    else if (j.contains("ti"))
    {
        std::string const& data = Base64Decode(
            j["ti"].get<std::string>());

        lt::error_code ec;
        lt::bdecode_node node = lt::bdecode(data, ec);

        if (ec)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to bdecode torrent: " << ec.message();
            return Error(1, ec.message());
        }

        p.ti = std::make_shared<lt::torrent_info>(node);
    }
    else
    {
        return Error(2, "Required field 'magnet_uri' or 'ti' not found");
    }

    p.save_path = j["save_path"].get<std::string>();

    if (auto session = m_session.lock())
    {
        return Ok({
            { "info_hash", session->AddTorrent(p) }
        });
    }

    return Error(99, "Failed to lock session");
}
