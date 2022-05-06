#include "sessionparams.hpp"

#include <libtorrent/session.hpp>

#include "../statement.hpp"

using pika::Data::Statement;
using pika::Data::Models::SessionParams;

libtorrent::session_params SessionParams::GetLatest(sqlite3 *db)
{
    /*
     * Statement::ForEach(
        db,
        "SELECT data FROM session_params ORDER BY timestamp DESC LIMIT 1",
        [&](Statement::Row const& row)
        {
            auto buffer = row.GetBlob(0);

            lt::error_code ec;
            lt::bdecode_node node = lt::bdecode(
                lt::span<const char>(buffer.data(), buffer.size()),
                ec);

            if (ec)
            {
                BOOST_LOG_TRIVIAL(warning) << "Failed to bdecode session params buffer: " << ec.message();
            }
            else
            {
                params = lt::read_session_params(node, lt::session::save_dht_state);
                BOOST_LOG_TRIVIAL(info) << "Loaded session params (" << buffer.size() << " bytes)";
            }
        });
     */
    return {};
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
            sqlite3_bind_blob(stmt, 1, buf.data(), static_cast<int>(buf.size()), SQLITE_TRANSIENT);
        });
}
