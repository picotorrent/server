#include "websocketsession.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>
#include <nlohmann/json.hpp>

#include "../sessionmanager.hpp"

using json = nlohmann::json;
using pt::Server::Http::WebSocketSession;
using pt::Server::SessionManager;

WebSocketSession::WebSocketSession(
    boost::asio::ip::tcp::socket&& socket,
    sqlite3* db,
    std::shared_ptr<SessionManager> const& session)
    : m_websocket(std::move(socket)),
    m_db(db),
    m_session(session)
{
    m_subscriberTag = m_session->Subscribe(
        std::bind(&WebSocketSession::OnSubscribe, this, std::placeholders::_1));
}

void WebSocketSession::BeginAccept(boost::system::error_code ec)
{
    if (ec)
    {
        return;
    }

    json j;
    j["type"] = "init";
    j["torrents"] = json::object();

    m_session->ForEachTorrent(
        [&j](auto const& ts)
        {
            std::stringstream ss;
            if (ts.info_hashes.has_v2()) { ss << ts.info_hashes.v2; }
            else { ss << ts.info_hashes.v1; }

            json torrent;
            torrent["name"] = ts.name;
            torrent["progress"] = ts.progress;
            torrent["save_path"] = ts.save_path;
            torrent["size_wanted"] = ts.total_wanted;
            torrent["state"] = ts.state;
            torrent["dl"] = ts.download_payload_rate;
            torrent["ul"] = ts.upload_payload_rate;
            torrent["info_hash"] = ss.str();

            j["torrents"][ss.str()] = torrent;

            return true;
        });

    BOOST_LOG_TRIVIAL(debug) << "Sending initial state to client";

    m_sendData.push(j.dump());

    MaybeWrite();
    BeginRead();
}

void WebSocketSession::BeginRead()
{
    m_websocket.async_read(
        m_buffer,
        boost::beast::bind_front_handler(
            &WebSocketSession::EndRead,
            shared_from_this()));
}

void WebSocketSession::EndRead(boost::system::error_code ec, std::size_t bytes_transferred)
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

void WebSocketSession::EndWrite(boost::beast::error_code ec, std::size_t bytes_transferred)
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

void WebSocketSession::MaybeWrite()
{
    if (m_isWriting
        || m_sendData.empty()) { return; };

    std::string& d = m_sendData.front();

    m_websocket.async_write(
        boost::asio::buffer(d),
        boost::beast::bind_front_handler(
            &WebSocketSession::EndWrite,
            shared_from_this()));

    m_isWriting = true;
}

void WebSocketSession::OnSubscribe(json& j)
{
    m_sendData.push(j.dump());
    MaybeWrite();
}
