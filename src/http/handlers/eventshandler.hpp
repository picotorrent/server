#pragma once

#include <memory>
#include <vector>

#include <boost/asio.hpp>

#include "../httprequesthandler.hpp"
#include "../../sessioneventhandler.hpp"

namespace pika::Http::Handlers
{
    class EventsHandler
        : public pika::Http::HttpRequestHandler
        , public pika::ISessionEventHandler
    {
    public:
        explicit EventsHandler(boost::asio::io_context& io);
        void Execute(std::shared_ptr<HttpRequestHandler::Context> context) override;

        // Session events
        void OnSessionStats(const std::map<std::string, int64_t> &stats) override;
        void OnTorrentAdded(const std::shared_ptr<ITorrentHandle>& handle) override;
        void OnTorrentRemoved(const lt::info_hash_t& hash) override;

    private:
        void Broadcast(const std::string& name, const std::string& data);
        void OnHeartbeatExpired(boost::system::error_code ec);

        class ContextState;

        boost::asio::steady_timer m_heartbeat;
        std::vector<std::shared_ptr<ContextState>> m_ctxs;
    };
}
