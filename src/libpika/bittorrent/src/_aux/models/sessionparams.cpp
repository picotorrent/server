#include "sessionparams.hpp"

#include <boost/log/trivial.hpp>
#include <libpika/data/database.hpp>
#include <libpika/data/row.hpp>
#include <libpika/data/statement.hpp>
#include <libtorrent/session.hpp>

namespace lt = libtorrent;
using libpika::bittorrent::_aux::models::SessionParams;

libtorrent::session_params SessionParams::GetLatest(libpika::data::Database& db)
{
    auto row = db.PrepareStatement("SELECT data FROM sessionparams ORDER BY timestamp DESC LIMIT 1")
        ->Step();

    lt::error_code ec;
    lt::bdecode_node node = lt::bdecode(
        row->GetBuffer(0),
        ec);

    if (ec)
    {
        BOOST_LOG_TRIVIAL(warning) << "Failed to bdecode session params buffer: " << ec.message();
        return {};
    }

    return lt::read_session_params(node, lt::session::save_dht_state);
}

void SessionParams::Insert(libpika::data::Database& db, const libtorrent::session_params &params)
{
    std::vector<char> buf = lt::write_session_params_buf(
        params,
        lt::session::save_dht_state);

    auto stmt = db.PrepareStatement("INSERT INTO sessionparams (data, timestamp) VALUES ($1, strftime('%s'));");
    stmt->Bind(1, buf);
    stmt->Step();
}
