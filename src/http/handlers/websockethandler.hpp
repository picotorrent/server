#pragma once

#include "../httprequesthandler.hpp"

namespace pika
{
    class Session;
}

namespace pika::Http::Handlers
{
    class WebSocketHandler : public HttpRequestHandler
    {
    public:
        explicit WebSocketHandler(std::shared_ptr<pika::Session> session);

        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

    private:
        std::shared_ptr<pika::Session> m_session;
    };
}
