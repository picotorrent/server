#pragma once

#include <memory>

#include <libpika/jsonrpc/method.hpp>
#include <nlohmann/json.hpp>

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionAddTorrentCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit SessionAddTorrentCommand(ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        ISession& m_session;
    };
}
