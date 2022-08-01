#pragma once

#include <libpika/jsonrpc/method.hpp>

namespace libpika::bittorrent { class ISession; }

namespace pika::RPC
{
    class SessionFindTorrents : public libpika::jsonrpc::Method
    {
    public:
        explicit SessionFindTorrents(libpika::bittorrent::ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        libpika::bittorrent::ISession& m_session;
    };
}