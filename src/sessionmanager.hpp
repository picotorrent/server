#pragma once

#include <map>
#include <memory>
#include <queue>
#include <vector>

#include <boost/asio.hpp>
#include <libtorrent/session.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace pt::Server::TSDB
{
    struct TimeSeriesDatabase;
}

namespace pt::Server
{
    class ITorrentHandleActor
    {
    public:
        virtual ~ITorrentHandleActor() = default;

        virtual bool IsValid() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
    };

    class ITorrentHandleFinder
    {
    public:
        virtual std::shared_ptr<ITorrentHandleActor> Find(const libtorrent::info_hash_t& hash) = 0;
    };

    class SessionManager : public ITorrentHandleFinder
    {
    public:
        static std::shared_ptr<SessionManager> Load(boost::asio::io_context& io, sqlite3* db, std::shared_ptr<TSDB::TimeSeriesDatabase> tsdb);

        ~SessionManager();

        // inherited
        std::shared_ptr<ITorrentHandleActor> Find(const libtorrent::info_hash_t& hash) override;

        libtorrent::info_hash_t AddTorrent(libtorrent::add_torrent_params& params);
        bool FindTorrent(libtorrent::info_hash_t const& hash, libtorrent::torrent_status& status);
        void ForEachTorrent(std::function<bool(libtorrent::torrent_status const& ts)> const&);
        void ReloadSettings();
        void RemoveTorrent(libtorrent::info_hash_t const& hash, bool removeFiles = false);
        std::shared_ptr<void> Subscribe(std::function<void(nlohmann::json&)>);

        SessionManager(boost::asio::io_context& io, sqlite3* db, std::unique_ptr<libtorrent::session> session, std::shared_ptr<TSDB::TimeSeriesDatabase> tsdb);

    private:
        void Broadcast(nlohmann::json&);
        void LoadTorrents();
        void ReadAlerts();
        void PostUpdates(boost::system::error_code ec);

        boost::asio::io_context& m_io;
        boost::asio::deadline_timer m_timer;

        sqlite3* m_db;
        std::unique_ptr<libtorrent::session> m_session;
        std::map<libtorrent::info_hash_t, libtorrent::torrent_status> m_torrents;
        std::vector<std::weak_ptr<std::function<void(nlohmann::json&)>>> m_subscribers;
        std::vector<libtorrent::stats_metric> m_stats;
        std::shared_ptr<TSDB::TimeSeriesDatabase> m_tsdb;
    };
}
