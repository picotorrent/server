#pragma once

#include <libpika/jsonrpc/method.hpp>

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsResumeCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsResumeCommand(ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        ISession& m_session;
    };
}
