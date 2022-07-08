#pragma once

#include <memory>
#include <vector>

#include <boost/asio.hpp>

#include "../context.hpp"
#include "../../sessioneventhandler.hpp"

namespace pika
{
    class ISession;
}

namespace pika::Http::Handlers
{
    class EventsHandler
        : public pika::ISessionEventHandler
    {
    public:
        explicit EventsHandler(boost::asio::io_context& io, std::weak_ptr<ISession> session);

        void operator()(std::shared_ptr<Http::Context> ctx);

        // Session events
        void OnSessionStats(const std::map<std::string, int64_t> &stats) override;
        void OnStateUpdate(const std::vector<std::shared_ptr<ITorrentHandle>> &) override;
        void OnTorrentAdded(const std::shared_ptr<ITorrentHandle>& handle) override;
        void OnTorrentPaused(const lt::info_hash_t& hash) override;
        void OnTorrentRemoved(const lt::info_hash_t& hash) override;
        void OnTorrentResumed(const lt::info_hash_t& hash) override;

    private:
        void Broadcast(const std::string& name, const std::string& data);
        void OnHeartbeatExpired(boost::system::error_code ec);

        class ContextState;

        boost::asio::steady_timer m_heartbeat;
        std::vector<std::shared_ptr<ContextState>> m_ctxs;
        std::weak_ptr<ISession> m_session;
    };
}
