#pragma once

#include <map>
#include <memory>
#include <queue>
#include <vector>

#include <boost/asio.hpp>
#include <libtorrent/session.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <toml++/toml.h>

namespace pika
{
    class ISessionEventHandler;
    class ITorrentHandle;

    class ISession
    {
    public:
        virtual void AddEventHandler(std::weak_ptr<ISessionEventHandler> handler) = 0;
        virtual std::map<std::string, int64_t> Counters() = 0;
        virtual libtorrent::info_hash_t AddTorrent(const libtorrent::add_torrent_params &params) = 0;
        virtual std::shared_ptr<ITorrentHandle> FindTorrent(const libtorrent::info_hash_t& hash) = 0;
        virtual void ForEachTorrent(const std::function<void(const libtorrent::torrent_status&)> &cb) = 0;
        virtual void RemoveTorrent(libtorrent::info_hash_t const& hash, bool removeFiles) = 0;
    };

    class Session : public ISession
    {
    public:
        static std::shared_ptr<Session> Load(
            boost::asio::io_context& io,
            sqlite3* db,
            const toml::table& config);

        ~Session();

        void AddEventHandler(std::weak_ptr<ISessionEventHandler> handler) override;
        libtorrent::info_hash_t AddTorrent(const libtorrent::add_torrent_params& params) override;
        std::map<std::string, int64_t> Counters() override;
        std::shared_ptr<ITorrentHandle> FindTorrent(const libtorrent::info_hash_t& hash) override;
        void ForEachTorrent(const std::function<void(const libtorrent::torrent_status&)> &cb) override;
        void RemoveTorrent(libtorrent::info_hash_t const& hash, bool removeFiles) override;

    private:
        Session(boost::asio::io_context& io, sqlite3* db, std::unique_ptr<libtorrent::session> session);

        std::map<std::string, std::string>& GetOrCreateLabelsMap(const libtorrent::info_hash_t& hash);
        void ReadAlerts();
        void PostUpdates(boost::system::error_code ec);
        void TriggerEvent(const std::function<void(ISessionEventHandler*)> &func);

        boost::asio::io_context& m_io;
        boost::asio::deadline_timer m_timer;

        sqlite3* m_db;
        std::unique_ptr<libtorrent::session> m_session;
        std::map<std::string, int64_t> m_metrics;
        std::map<libtorrent::info_hash_t, libtorrent::torrent_status> m_torrents;
        std::map<libtorrent::info_hash_t, std::map<std::string, std::string>> m_labels;
        std::vector<libtorrent::stats_metric> m_stats;
        std::vector<std::weak_ptr<ISessionEventHandler>> m_eventHandlers;
    };
}
