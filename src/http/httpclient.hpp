#pragma once

#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace pika::Http
{
    class HttpClient : public std::enable_shared_from_this<HttpClient>
    {
    public:
        explicit HttpClient(boost::asio::io_context &io);

        void Send(
            const std::string_view &url);

    private:
        void OnResolve(
            boost::beast::error_code ec,
            const boost::asio::ip::tcp::resolver::results_type &results);

        void OnConnect(
            boost::beast::error_code ec,
            const boost::asio::ip::tcp::resolver::endpoint_type &endpoint);

        void OnWrite(
            boost::beast::error_code ec,
            size_t bytes);

        void OnRead(
            boost::beast::error_code ec,
            size_t bytes);

        boost::asio::ip::tcp::resolver m_resolver;
        boost::beast::tcp_stream m_stream;
        boost::beast::http::request<boost::beast::http::string_body> m_req;
        boost::beast::http::response<boost::beast::http::string_body> m_res;
        boost::beast::flat_buffer m_buffer;
    };
}
