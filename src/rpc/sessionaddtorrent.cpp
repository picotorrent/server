#include "sessionaddtorrent.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/bdecode.hpp>
#include <libtorrent/torrent_info.hpp>

#include "../json/infohash.hpp"
#include "../sessionmanager.hpp"

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
using pt::Server::SessionManager;
using pt::Server::RPC::SessionAddTorrentCommand;

SessionAddTorrentCommand::SessionAddTorrentCommand(std::shared_ptr<SessionManager> session)
    : m_session(std::move(session))
{
}

json SessionAddTorrentCommand::Execute(const json& j)
{
    if (!j.contains("data"))
    {
        return Error(1, "Missing 'data' field");
    }

    if (!j.contains("save_path"))
    {
        return Error(1, "Missing 'save_path' field");
    }

    std::string const& data = Base64Decode(
        j["data"].get<std::string>());

    lt::error_code ec;
    lt::bdecode_node node = lt::bdecode(data, ec);

    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to bdecode torrent: " << ec.message();
        return Error(1, ec.message());
    }

    lt::add_torrent_params p;
    p.save_path = j["save_path"].get<std::string>();
    p.ti = std::make_shared<lt::torrent_info>(node);

    return Ok({
        { "info_hash", m_session->AddTorrent(p) }
    });
}
