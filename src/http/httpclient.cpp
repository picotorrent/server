#include "httpclient.hpp"

#include <boost/log/trivial.hpp>
#include <uriparser/Uri.h>

using pika::Http::HttpClient;

HttpClient::HttpClient(boost::asio::io_context &io)
    : m_resolver(boost::asio::make_strand(io))
    , m_stream(boost::asio::make_strand(io))
{
}

void HttpClient::Send(
    const std::string_view &url)
{
    UriUriA parsed;
    const char *err;

    uriParseSingleUriA(&parsed, url.data(), &err);

    std::string host = std::string(parsed.hostText.first, parsed.hostText.afterLast);

    m_resolver.async_resolve(
        host,
        std::string(parsed.portText.first, parsed.portText.afterLast),
        boost::beast::bind_front_handler(
            &HttpClient::OnResolve,
            shared_from_this()));

    uriFreeUriMembersA(&parsed);
}

void HttpClient::OnResolve(
    boost::beast::error_code ec,
    const boost::asio::ip::tcp::resolver::results_type &results)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Resolve failed: " << ec;
        return;
    }

    m_stream.async_connect(
        results,
        boost::beast::bind_front_handler(
            &HttpClient::OnConnect,
            shared_from_this()));
}

void HttpClient::OnConnect(
    boost::beast::error_code ec,
    const boost::asio::ip::tcp::resolver::endpoint_type &endpoint)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Connected failed " << endpoint;
        return;
    }

    BOOST_LOG_TRIVIAL(debug) << "Connected to " << endpoint;

    m_req.version(10);
    m_req.method(boost::beast::http::verb::post);
    m_req.target("/events");
    m_req.set(boost::beast::http::field::host, "localhost");
    m_req.set(boost::beast::http::field::user_agent, "pika/1.0");
    m_req.body() = "m_buffer.data();";
    m_req.prepare_payload();

    boost::beast::http::async_write(
        m_stream,
        m_req,
        boost::beast::bind_front_handler(
            &HttpClient::OnWrite,
            shared_from_this()));
}

void HttpClient::OnWrite(boost::beast::error_code ec, size_t bytes)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << ec;
        return;
    }

    boost::beast::http::async_read(
        m_stream,
        m_buffer,
        m_res,
        boost::beast::bind_front_handler(
            &HttpClient::OnRead,
            shared_from_this()));
}

void HttpClient::OnRead(boost::beast::error_code ec, size_t bytes)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Read failed: " << ec;
        return;
    }
    BOOST_LOG_TRIVIAL(info) << ec;
    BOOST_LOG_TRIVIAL(info) << bytes << " bytes";
}
