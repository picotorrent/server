#pragma once

#include <map>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <libtorrent/fwd.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace pt::Server
{
    class SessionManager
    {
    public:
        static std::shared_ptr<SessionManager> Load(boost::asio::io_context& io, sqlite3* db);

        ~SessionManager();

        libtorrent::info_hash_t AddTorrent(libtorrent::add_torrent_params& params);
        bool FindTorrent(libtorrent::info_hash_t const& hash, libtorrent::torrent_status& status);
        void ForEachTorrent(std::function<bool(libtorrent::torrent_status const& ts)> const&);
        void ReloadSettings();
        void RemoveTorrent(libtorrent::info_hash_t const& hash, bool removeFiles = false);
        std::shared_ptr<void> Subscribe(std::function<void(nlohmann::json&)>);

    private:
        SessionManager(boost::asio::io_context& io, sqlite3* db, std::unique_ptr<libtorrent::session> session);

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
    };
}
