#pragma once

#include <libpika/jsonrpc/method.hpp>

namespace libpika::bittorrent { class ISession; }

namespace pika::RPC
{
    class TorrentsPauseCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsPauseCommand(libpika::bittorrent::ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        libpika::bittorrent::ISession& m_session;
    };
}
