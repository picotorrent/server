#pragma once

#include <map>
#include <memory>
#include <queue>
#include <vector>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <libtorrent/session.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <toml++/toml.h>

namespace pika
{
    class ITorrentHandle;

    class ISession
    {
    public:
        typedef boost::signals2::signal<void(const lt::info_hash_t&)> InfoHashSignal;
        typedef boost::signals2::signal<void(const std::map<std::string, int64_t>&)> SessionStatsSignal;
        typedef boost::signals2::signal<void(const std::shared_ptr<ITorrentHandle>&)> TorrentHandleSignal;
        typedef boost::signals2::signal<void(const std::vector<std::shared_ptr<ITorrentHandle>>&)> TorrentHandleListSignal;

        // Events
        virtual boost::signals2::connection OnSessionStats(const SessionStatsSignal::slot_type& subscriber) = 0;
        virtual boost::signals2::connection OnStateUpdate(const TorrentHandleListSignal::slot_type& subscriber) = 0;
        virtual boost::signals2::connection OnTorrentAdded(const TorrentHandleSignal::slot_type& subscriber) = 0;
        virtual boost::signals2::connection OnTorrentPaused(const InfoHashSignal::slot_type& subscriber) = 0;
        virtual boost::signals2::connection OnTorrentRemoved(const InfoHashSignal::slot_type& subscriber) = 0;
        virtual boost::signals2::connection OnTorrentResumed(const InfoHashSignal::slot_type& subscriber) = 0;

        virtual std::map<std::string, int64_t> Counters() = 0;
        virtual libtorrent::info_hash_t AddTorrent(const libtorrent::add_torrent_params &params) = 0;
        virtual std::shared_ptr<ITorrentHandle> FindTorrent(const libtorrent::info_hash_t& hash) = 0;
        virtual void ForEachTorrent(const std::function<void(const libtorrent::torrent_status&)> &cb) = 0;
        virtual void RemoveTorrent(libtorrent::info_hash_t const& hash, bool removeFiles) = 0;
    };

    class Session : public ISession
    {
    public:
        struct Options
        {
            lt::settings_pack settings;
        };

        explicit Session(boost::asio::io_context& io, sqlite3* db, const Options& opts);

        static std::shared_ptr<Session> Load(
            boost::asio::io_context& io,
            sqlite3* db,
            const toml::table& config);

        ~Session();

        boost::signals2::connection OnSessionStats(const SessionStatsSignal::slot_type& subscriber) override
        {
            return m_sessionStats.connect(subscriber);
        }

        boost::signals2::connection OnStateUpdate(const TorrentHandleListSignal::slot_type& subscriber) override
        {
            return m_stateUpdate.connect(subscriber);
        }

        boost::signals2::connection OnTorrentAdded(const TorrentHandleSignal::slot_type& subscriber) override
        {
            return m_torrentAdded.connect(subscriber);
        }

        boost::signals2::connection OnTorrentPaused(const InfoHashSignal::slot_type& subscriber) override
        {
            return m_torrentPaused.connect(subscriber);
        }

        boost::signals2::connection OnTorrentRemoved(const InfoHashSignal::slot_type& subscriber) override
        {
            return m_torrentRemoved.connect(subscriber);
        }

        boost::signals2::connection OnTorrentResumed(const InfoHashSignal::slot_type& subscriber) override
        {
            return m_torrentResumed.connect(subscriber);
        }

        libtorrent::info_hash_t AddTorrent(const libtorrent::add_torrent_params& params) override;
        std::map<std::string, int64_t> Counters() override;
        std::shared_ptr<ITorrentHandle> FindTorrent(const libtorrent::info_hash_t& hash) override;
        void ForEachTorrent(const std::function<void(const libtorrent::torrent_status&)> &cb) override;
        void RemoveTorrent(libtorrent::info_hash_t const& hash, bool removeFiles) override;

    private:
        std::map<std::string, std::string>& GetOrCreateLabelsMap(const libtorrent::info_hash_t& hash);
        void ReadAlerts();
        void PostUpdates(boost::system::error_code ec);

        boost::asio::io_context& m_io;
        boost::asio::deadline_timer m_timer;

        SessionStatsSignal m_sessionStats;
        TorrentHandleListSignal m_stateUpdate;
        TorrentHandleSignal m_torrentAdded;
        InfoHashSignal m_torrentPaused;
        InfoHashSignal m_torrentRemoved;
        InfoHashSignal m_torrentResumed;

        sqlite3* m_db;
        std::unique_ptr<libtorrent::session> m_session;
        std::map<std::string, int64_t> m_metrics;
        std::map<libtorrent::info_hash_t, libtorrent::torrent_status> m_torrents;
        std::map<libtorrent::info_hash_t, std::map<std::string, std::string>> m_labels;
        std::vector<libtorrent::stats_metric> m_stats;
    };
}
