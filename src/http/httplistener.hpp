#pragma once

#include <map>
#include <memory>

#include <boost/asio.hpp>
#include <sqlite3.h>

namespace pt::Server { class SessionManager; }
namespace pt::Server::RPC { class Command; }

namespace pt::Server::Http
{
    class HttpListener : public std::enable_shared_from_this<HttpListener>
    {
    public:
        HttpListener(
            boost::asio::io_context& ioc,
            boost::asio::ip::tcp::endpoint endpoint,
            sqlite3* db,
            std::shared_ptr<SessionManager> const& session,
            std::shared_ptr<std::string const> const& docroot);

        std::map<std::string, std::shared_ptr<pt::Server::RPC::Command>>& Commands();
        void Run();

    private:
        void BeginAccept();
        void EndAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

        boost::asio::io_context& m_io;
        boost::asio::ip::tcp::acceptor m_acceptor;
        sqlite3* m_db;
        std::shared_ptr<SessionManager> m_session;
        std::shared_ptr<std::string const> m_docroot;
        std::shared_ptr<std::map<std::string, std::shared_ptr<pt::Server::RPC::Command>>> m_commands;
    };
}
