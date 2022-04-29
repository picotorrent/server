#include "websockethandler.hpp"

#include <boost/beast.hpp>
#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>
#include <utility>

#include "../../json/torrentstatus.hpp"
#include "../../session.hpp"

using json = nlohmann::json;
using pika::Http::Handlers::WebSocketHandler;
using pika::Session;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    WebSocketSession(boost::asio::ip::tcp::socket &&socket, std::shared_ptr<Session> session)
        : m_websocket(std::move(socket)),
          m_session(std::move(session))
    {
        m_subscriberTag = m_session->Subscribe(
            [this](auto && PH1) { OnSubscribe(std::forward<decltype(PH1)>(PH1)); });
    }

    template<class Body, class Allocator>
    void Run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
    {
        // Set suggested timeout settings for the websocket
        m_websocket.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        m_websocket.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::response_type& res)
                {
                    res.set(boost::beast::http::field::server, "pika/1.0");
                }));

        // Accept the websocket handshake
        m_websocket.async_accept(
                req,
                boost::beast::bind_front_handler(
                        &WebSocketSession::BeginAccept,
                        shared_from_this()));
    }

private:
    void BeginAccept(boost::system::error_code ec)
    {
        if (ec)
        {
            BOOST_LOG_TRIVIAL(error) << "Error when accepting WebSocket: " << ec.message();
            return;
        }

        json j;
        j["type"] = "init";
        j["torrents"] = json::object();

        m_session->ForEachTorrent(
            [&j](lt::torrent_status const &ts)
            {
                std::stringstream ss;
                if (ts.info_hashes.has_v2()) { ss << ts.info_hashes.v2; }
                else { ss << ts.info_hashes.v1; }

                j["torrents"][ss.str()] = ts;

                return true;
            });

        BOOST_LOG_TRIVIAL(debug) << "Sending initial state to client";

        m_sendData.push(j.dump());

        MaybeWrite();
        BeginRead();
    }

    void BeginRead()
    {
        m_websocket.async_read(
            m_buffer,
            boost::beast::bind_front_handler(
                &WebSocketSession::EndRead,
                shared_from_this()));
    }

    void EndRead(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        if (ec == boost::beast::websocket::error::closed)
        {
            return;
        }

        if (ec)
        {
            BOOST_LOG_TRIVIAL(error) << "Error when reading from WebSocket: " << ec.message();
            return;
        }

        // We only ignore the read data. If it should be processed, do that here.

        BeginRead();
    }

    void EndWrite(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        m_isWriting = false;
        m_sendData.pop();

        if (ec)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to write " << bytes_transferred << " bytes: " << ec;
            return;
        }

        MaybeWrite();
    }

    void MaybeWrite()
    {
        if (m_isWriting || m_sendData.empty()) { return; }

        std::string &d = m_sendData.front();

        m_websocket.async_write(
            boost::asio::buffer(d),
            boost::beast::bind_front_handler(
                &WebSocketSession::EndWrite,
                shared_from_this()));

        m_isWriting = true;
    }

    void OnSubscribe(json &j)
    {
        m_sendData.push(j.dump());
        MaybeWrite();
    }

    boost::beast::websocket::stream<boost::beast::tcp_stream> m_websocket;
    boost::beast::flat_buffer m_buffer;
    std::shared_ptr<Session> m_session;
    bool m_isWriting { false };
    std::queue<std::string> m_sendData;
    std::shared_ptr<void> m_subscriberTag;
};

WebSocketHandler::WebSocketHandler(std::shared_ptr<pika::Session> session)
    : m_session(std::move(session))
{
}

void WebSocketHandler::Execute(std::shared_ptr<HttpRequestHandler::Context> context)
{
    std::make_shared<WebSocketSession>(
        context->Stream().release_socket(),
        m_session)->Run(context->Request());
}
