#pragma once

#include "../httprequesthandler.hpp"

namespace pika::Http::Handlers
{
    class WebRootHandler : public HttpRequestHandler
    {
    public:
        explicit WebRootHandler(std::string root, std::string virtualPath);
        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

    private:
        std::string m_root;
        std::string m_virtualPath;
    };
}
