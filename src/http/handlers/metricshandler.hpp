#pragma once

#include "../httprequesthandler.hpp"

namespace pika::EventHandlers { class SessionStatsHandler; }

namespace pika::Http::Handlers
{
    class MetricsHandler : public HttpRequestHandler
    {
    public:
        explicit MetricsHandler(std::weak_ptr<pika::EventHandlers::SessionStatsHandler> ssh);

        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

    private:
        std::weak_ptr<pika::EventHandlers::SessionStatsHandler> m_ssh;
    };
}
