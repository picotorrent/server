#pragma once

#include <libpika/jsonrpc/method.hpp>
#include <nlohmann/json.hpp>

namespace pika { class ISession; }

namespace pika::RPC
{
    class SessionStatsCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit SessionStatsCommand(ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        ISession& m_session;
    };
}
