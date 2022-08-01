#pragma once

#include <libtorrent/session_params.hpp>

namespace libpika::data
{
    class Database;
}

namespace libpika::bittorrent::_aux::models
{
    class SessionParams
    {
    public:
        static libtorrent::session_params GetLatest(libpika::data::Database& db);
        static void Insert(libpika::data::Database& db, const libtorrent::session_params &params);
    };
}
