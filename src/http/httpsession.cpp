#include "httpsession.hpp"

#include <filesystem>

#include <memory>
#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>
#include <utility>

#include "httprequesthandler.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;
using pika::Http::HttpRequestHandler;
using pika::Http::HttpSession;
using pika::SessionManager;

using BasicHttpRequest = boost::beast::http::request<boost::beast::http::string_body>;

class HttpSession::DefaultContext : public pika::Http::HttpRequestHandler::Context
{
public:
    explicit DefaultContext(std::shared_ptr<HttpSession> session, BasicHttpRequest request)
        : m_session(std::move(session)),
        m_req(std::move(request))
    {
    }

    BasicHttpRequest& Request() override
    {
        return m_req;
    }

    boost::beast::tcp_stream& Stream() override
    {
        return m_session->m_stream;
    }

    void Write(boost::beast::http::response<boost::beast::http::string_body> res) override
    {
        m_session->m_queue(std::move(res));
    }

private:
    std::shared_ptr<HttpSession> m_session;
    BasicHttpRequest m_req;
};

HttpSession::HttpSession(
    boost::asio::ip::tcp::socket&& socket,
    std::shared_ptr<std::map<std::tuple<std::string, std::string>, std::shared_ptr<HttpRequestHandler>>> handlers)
    : m_stream(std::move(socket))
    , m_handlers(std::move(handlers))
    , m_queue(*this)
{
}

void HttpSession::Run()
{
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    boost::asio::dispatch(
        m_stream.get_executor(),
        boost::beast::bind_front_handler(
            &HttpSession::BeginRead,
            shared_from_this()));
}

void HttpSession::BeginRead()
{
    m_parser.emplace();
    m_parser->body_limit(10000000);

    m_stream.expires_after(std::chrono::seconds(30));

    // Read a request using the parser-oriented interface
    boost::beast::http::async_read(
        m_stream,
        m_buffer,
        *m_parser,
        boost::beast::bind_front_handler(
            &HttpSession::EndRead,
            shared_from_this()));
}

void HttpSession::EndRead(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    namespace http = boost::beast::http;

    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if(ec == http::error::end_of_stream)
    {
        return BeginClose();
    }

    if(ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Error when reading HTTP request: " << ec.message();
        return;
    }

    // Check for matching handler
    auto req = m_parser->release();
    auto method = req.method_string();
    auto path = req.target();
    auto handler = m_handlers->find({ method.to_string(), path.to_string() });

    if (handler != m_handlers->end())
    {
        handler->second->Execute(
            std::make_shared<DefaultContext>(
                shared_from_this(),
                std::move(req)));
        return;
    }

    auto const not_found = [&req](boost::beast::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    m_queue(not_found(req.target()));

    // If we aren't at the queue limit, try to pipeline another request
    if(!m_queue.IsFull())
    {
        BeginRead();
    }
}

void HttpSession::EndWrite(bool close, boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        // TODO (log)
        return;
    }

    if (close)
    {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return BeginClose();
    }

    // Inform the queue that a write completed
    if (m_queue.OnWrite())
    {
        // Read another request
        BeginRead();
    }
}

void HttpSession::BeginClose()
{
    // Send a TCP shutdown
    boost::beast::error_code ec;
    m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}
