#pragma once

#include <memory>
#include <queue>
#include <string>

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace pt::Server { class SessionManager; }

namespace pt::Server::Http
{
    class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
    {
    public:
        explicit WebSocketSession(
            boost::asio::ip::tcp::socket&& socket,
            sqlite3* db,
            std::shared_ptr<SessionManager> const& session);

        template<class Body, class Allocator>
        void Run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
        {
            // Set suggested timeout settings for the websocket
            m_websocket.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

            // Set a decorator to change the Server of the handshake
            m_websocket.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::response_type& res)
                {
                    res.set(boost::beast::http::field::server, "PicoTorrentServer/1.0");
                }));

            // Accept the websocket handshake
            m_websocket.async_accept(
                req,
                boost::beast::bind_front_handler(
                    &WebSocketSession::BeginAccept,
                    shared_from_this()));
        }

    private:
        void BeginAccept(boost::system::error_code ec);
        void BeginRead();
        void EndRead(boost::system::error_code ec, size_t bytes_transferred);
        void EndWrite(boost::system::error_code ec, size_t bytes_transferred);
        void MaybeWrite();
        void OnSubscribe(nlohmann::json& j);

        boost::beast::websocket::stream<boost::beast::tcp_stream> m_websocket;
        boost::beast::flat_buffer m_buffer;
        sqlite3* m_db;
        std::shared_ptr<SessionManager> m_session;
        bool m_isWriting { false };
        std::queue<std::string> m_sendData;
        std::shared_ptr<void> m_subscriberTag;
    };
}
