#pragma once

#include <map>
#include <memory>

#include <boost/asio.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/settings_pack.hpp>
#include <sqlite3.h>

namespace pt::Server
{
    class ITorrentHandle
    {
    public:
        virtual bool IsValid() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
    };

    class ISession
    {
    public:
        virtual ~ISession() {}
        virtual lt::info_hash_t AddTorrent(lt::add_torrent_params& params) = 0;
        virtual void ApplySettings(const lt::settings_pack& settings) = 0;
        virtual std::shared_ptr<ITorrentHandle> FindTorrent(const lt::info_hash_t &hash) = 0;
        virtual void ForEachTorrent(std::function<bool(lt::torrent_status const& ts)> const& iteree) = 0;
        virtual void RemoveTorrent(lt::info_hash_t const& hash, bool remove_files) = 0;
    };

    class Session : public ISession
    {
    public:
        Session(boost::asio::io_context& io, sqlite3* db, int id, std::string name, const libtorrent::session_params& params);
        ~Session() override;

        void ApplySettings(const lt::settings_pack& settings) override;
        lt::info_hash_t AddTorrent(lt::add_torrent_params& params) override;
        std::shared_ptr<ITorrentHandle> FindTorrent(const lt::info_hash_t& hash) override;
        void ForEachTorrent(std::function<bool(lt::torrent_status const& ts)> const& iteree) override;
        void RemoveTorrent(lt::info_hash_t const& hash, bool remove_files) override;

    private:
        void PostUpdates(boost::system::error_code ec);
        void ReadAlerts();

        boost::asio::io_context& m_io;
        boost::asio::deadline_timer m_timer;

        sqlite3* m_db;
        std::unique_ptr<libtorrent::session> m_session;
        std::map<libtorrent::info_hash_t, libtorrent::torrent_status> m_torrents;
    };
}
