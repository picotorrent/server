#pragma once

#include <map>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <toml++/toml.h>

namespace pika::Http
{
    class Context;
    class HttpRequestHandler;
    class HttpSession;

    class HttpListener : public std::enable_shared_from_this<HttpListener>
    {
    public:
        HttpListener(
            boost::asio::io_context& ioc,
            const toml::table& config);

        ~HttpListener();

        void Run();
        void Stop();

        void Use(const std::function<void(std::shared_ptr<Context>)>& middleware);

    private:
        void BeginAccept();
        void EndAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

        boost::asio::io_context& m_io;
        boost::asio::ip::tcp::acceptor m_acceptor;
        std::vector<std::weak_ptr<HttpSession>> m_sessions;
        std::vector<std::function<void(std::shared_ptr<Context>)>> m_middlewares;
    };
}
