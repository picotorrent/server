#include "sessionparams.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/session.hpp>

#include "../statement.hpp"

namespace lt = libtorrent;
using pika::Data::Statement;
using pika::Data::Models::SessionParams;

libtorrent::session_params SessionParams::GetLatest(sqlite3 *db)
{
    lt::session_params params;

    Statement::ForEach(
        db,
    "SELECT data FROM sessionparams ORDER BY timestamp DESC LIMIT 1",
    [&params](const Statement::Row &row)
        {
            auto buf = row.GetBlob(0);

            lt::error_code ec;
            lt::bdecode_node node = lt::bdecode(buf,ec);

            if (ec)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to bdecode session params buffer: " << ec.message();
                return;
            }

            params = lt::read_session_params(node, lt::session::save_dht_state);
        });

    return params;
}

void SessionParams::Insert(sqlite3 *db, const libtorrent::session_params &params)
{
    std::vector<char> buf = lt::write_session_params_buf(
        params,
        lt::session::save_dht_state);

    Statement::ForEach(
        db,
        "INSERT INTO sessionparams (data, timestamp) VALUES ($1, strftime('%s'));",
        [](auto const&) {},
        [&buf](sqlite3_stmt* stmt)
        {
            CHECK_OK(sqlite3_bind_blob(stmt, 1, buf.data(), static_cast<int>(buf.size()), SQLITE_TRANSIENT))
            return SQLITE_OK;
        });
}
