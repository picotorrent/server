#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <libtorrent/settings_pack.hpp>
#include <sqlite3.h>

namespace pt::Server::Data::Models
{
    class Proxy
    {
    public:
        static std::shared_ptr<Proxy> Create(
            sqlite3* db,
            std::string const& name,
            libtorrent::settings_pack::proxy_type_t type,
            std::string const& hostname,
            int port,
            std::optional<std::string> const& username,
            std::optional<std::string> const& password,
            bool proxyHostnames,
            bool proxyPeerConnections,
            bool proxyTrackerConnections);

        static std::vector<std::shared_ptr<Proxy>> GetAll(sqlite3* db);
        static std::shared_ptr<Proxy> GetById(sqlite3* db, int proxyId);

        int Id() { return m_id; }

    private:
        int m_id;
    };
}
