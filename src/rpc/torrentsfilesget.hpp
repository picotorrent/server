#pragma once

#include <libpika/jsonrpc/method.hpp>

namespace libpika::bittorrent { class ISession; }

namespace pika::RPC
{
    class TorrentsFilesGetCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsFilesGetCommand(libpika::bittorrent::ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        libpika::bittorrent::ISession& m_session;
    };
}
