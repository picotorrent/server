#pragma once

#include <libpika/jsonrpc/method.hpp>

namespace libpika::bittorrent { class ISession; }

namespace pika::RPC
{
    class TorrentsMoveStorageCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsMoveStorageCommand(libpika::bittorrent::ISession&);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        libpika::bittorrent::ISession& m_session;
    };
}
