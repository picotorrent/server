#pragma once

#include "../httprequesthandler.hpp"

namespace pt::Server
{
    class SessionManager;
}

namespace pt::Server::Http::Handlers
{
    class WebSocketHandler : public HttpRequestHandler
    {
    public:
        explicit WebSocketHandler(std::shared_ptr<pt::Server::SessionManager>  sm);

        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

    private:
        std::shared_ptr<pt::Server::SessionManager> m_sm;
    };
}
