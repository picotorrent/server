#pragma once

#include "../httprequesthandler.hpp"

namespace pt::Server
{
    class ISessionManager;
}

namespace pt::Server::Http::Handlers
{
    class WebSocketHandler : public HttpRequestHandler
    {
    public:
        explicit WebSocketHandler(std::shared_ptr<pt::Server::ISessionManager>  sm);

        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

    private:
        std::shared_ptr<pt::Server::ISessionManager> m_sm;
    };
}
