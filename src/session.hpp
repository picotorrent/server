#pragma once

#include <map>
#include <memory>
#include <queue>
#include <vector>

#include <boost/asio.hpp>
#include <libtorrent/session.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace pika
{
    class ISessionEventHandler;

    class ITorrentHandle
    {
    public:
        virtual ~ITorrentHandle() = default;

        virtual bool IsValid() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
    };

    class ISession
    {
    public:
        virtual void AddEventHandler(std::shared_ptr<ISessionEventHandler> handler) = 0;
        virtual std::shared_ptr<ITorrentHandle> FindTorrent(const libtorrent::info_hash_t& hash) = 0;
    };

    class Session : public ISession
    {
    public:
        static std::shared_ptr<Session> Load(boost::asio::io_context& io, sqlite3* db);

        ~Session();

        // inherited
        void AddEventHandler(std::shared_ptr<ISessionEventHandler> handler) override;
        std::shared_ptr<ITorrentHandle> FindTorrent(const libtorrent::info_hash_t& hash) override;

        libtorrent::info_hash_t AddTorrent(libtorrent::add_torrent_params& params);
        void RemoveTorrent(libtorrent::info_hash_t const& hash, bool removeFiles = false);

    private:
        Session(boost::asio::io_context& io, sqlite3* db, std::unique_ptr<libtorrent::session> session);

        void ReadAlerts();
        void PostUpdates(boost::system::error_code ec);
        void TriggerEvent(const std::function<void(ISessionEventHandler*)> &func);

        boost::asio::io_context& m_io;
        boost::asio::deadline_timer m_timer;

        sqlite3* m_db;
        std::unique_ptr<libtorrent::session> m_session;
        std::map<libtorrent::info_hash_t, libtorrent::torrent_status> m_torrents;
        std::vector<std::weak_ptr<std::function<void(nlohmann::json&)>>> m_subscribers;
        std::vector<libtorrent::stats_metric> m_stats;

        std::vector<std::weak_ptr<ISessionEventHandler>> m_eventHandlers;
    };
}