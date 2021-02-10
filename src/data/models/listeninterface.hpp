#pragma once

#include <memory>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace pt::Server::Data::Models
{
    class ListenInterface
    {
    public:
        static std::shared_ptr<ListenInterface> Create(
            sqlite3* db,
            std::string const& host,
            int port,
            bool isLocal,
            bool isOutgoing,
            bool isSsl);

        static std::vector<std::shared_ptr<ListenInterface>> GetAll(sqlite3* db);

        int Id() { return m_id; }
        std::string Host() { return m_host; }
        int Port() { return m_port; }
        bool IsLocal() { return m_isLocal; }
        bool IsOutgoing() { return m_isOutgoing; }
        bool IsSsl() { return m_isSsl; }

    private:
        int m_id;
        std::string m_host;
        int m_port;
        bool m_isLocal;
        bool m_isOutgoing;
        bool m_isSsl;
    };
}
