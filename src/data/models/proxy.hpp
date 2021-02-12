#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <libtorrent/settings_pack.hpp>
#include <sqlite3.h>

#include "../statement.hpp"

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
        std::string Name() { return m_name; }
        libtorrent::settings_pack::proxy_type_t Type() { return m_type; }
        std::string Hostname() { return m_hostname; }
        int Port() { return m_port; }
        std::optional<std::string> Username() { return m_username; }
        std::optional<std::string> Password() { return m_password; }
        bool ProxyHostnames() { return m_proxyHostnames; }
        bool ProxyPeerConnections() { return m_proxyPeerConnections; }
        bool ProxyTrackerConnections() { return m_proxyTrackerConnections; }

    private:
        static std::shared_ptr<Proxy> Construct(Statement::Row const& row);

        int m_id;
        std::string m_name;
        libtorrent::settings_pack::proxy_type_t m_type;
        std::string m_hostname;
        int m_port;
        std::optional<std::string> m_username;
        std::optional<std::string> m_password;
        bool m_proxyHostnames;
        bool m_proxyPeerConnections;
        bool m_proxyTrackerConnections;
    };
}
