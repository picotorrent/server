#pragma once

#include <libpika/jsonrpc/method.hpp>

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsPauseCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsPauseCommand(ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        ISession& m_session;
    };
}
