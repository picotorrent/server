#pragma once

#include <libpika/jsonrpc/method.hpp>

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsMoveStorageCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsMoveStorageCommand(ISession&);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        ISession& m_session;
    };
}
