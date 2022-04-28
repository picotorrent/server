#pragma once

#include "../httprequesthandler.hpp"

namespace pt::Server
{
    class Session;
}

namespace pt::Server::Http::Handlers
{
    class WebSocketHandler : public HttpRequestHandler
    {
    public:
        explicit WebSocketHandler(std::shared_ptr<pt::Server::Session> session);

        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

    private:
        std::shared_ptr<pt::Server::Session> m_session;
    };
}
