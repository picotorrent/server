#pragma once

#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <libpika/http/context.hpp>
#include <libtorrent/info_hash.hpp>

namespace lt = libtorrent;

namespace libpika::bittorrent
{
    class ISession;
    class ITorrentHandle;
}

namespace pika::Http::Handlers
{
    class EventsHandler
    {
    public:
        explicit EventsHandler(boost::asio::io_context& io, libpika::bittorrent::ISession& session);
        explicit EventsHandler(const EventsHandler& eh);

        ~EventsHandler() noexcept;

        void operator()(const std::shared_ptr<libpika::http::Context>& ctx);

    private:
        void OnSessionStats(const std::map<std::string, int64_t> &stats);
        void OnStateUpdate(const std::vector<std::shared_ptr<libpika::bittorrent::ITorrentHandle>> &);
        void OnTorrentAdded(const std::shared_ptr<libpika::bittorrent::ITorrentHandle>& handle);
        void OnTorrentPaused(const lt::info_hash_t& hash);
        void OnTorrentRemoved(const lt::info_hash_t& hash);
        void OnTorrentResumed(const lt::info_hash_t& hash);

        void Broadcast(const std::string& name, const std::string& data);
        void OnHeartbeatExpired(boost::system::error_code ec);

        class ContextState;

        boost::asio::io_context& m_io;
        boost::asio::steady_timer m_heartbeat;
        std::vector<std::shared_ptr<ContextState>> m_ctxs;

        boost::signals2::connection m_sessionStatsConnection;
        boost::signals2::connection m_stateUpdateConnection;
        boost::signals2::connection m_torrentAddedConnection;

        libpika::bittorrent::ISession& m_session;
    };
}
