#pragma once

#include <map>
#include <memory>

#include <boost/asio.hpp>
#include <toml++/toml.h>

namespace pika::Http
{
    class HttpRequestHandler;
    class HttpSession;

    class HttpListener : public std::enable_shared_from_this<HttpListener>
    {
    public:
        HttpListener(
            boost::asio::io_context& ioc,
            const toml::table& config);

        ~HttpListener();

        void AddHandler(
            const std::string& method,
            const std::string& path,
            const std::shared_ptr<HttpRequestHandler>& handler);

        void Run();
        void Stop();

    private:
        void BeginAccept();
        void EndAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

        boost::asio::io_context& m_io;
        boost::asio::ip::tcp::acceptor m_acceptor;
        std::vector<std::weak_ptr<HttpSession>> m_sessions;
        std::shared_ptr<std::map<std::tuple<std::string, std::string>, std::shared_ptr<HttpRequestHandler>>> m_handlers;
    };
}
