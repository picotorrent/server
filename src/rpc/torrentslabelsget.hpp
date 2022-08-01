#pragma once

#include <libpika/jsonrpc/method.hpp>
#include <sqlite3.h>

namespace libpika::bittorrent { class ISession; }

namespace pika::RPC
{
    class TorrentsLabelsGetCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsLabelsGetCommand(libpika::bittorrent::ISession&);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        libpika::bittorrent::ISession& m_session;
    };
}
