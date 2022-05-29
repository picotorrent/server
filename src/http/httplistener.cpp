#include "httplistener.hpp"

#include <boost/log/trivial.hpp>
#include <utility>

#include "httpsession.hpp"

using pika::Http::HttpListener;
using pika::Http::HttpRequestHandler;

HttpListener::HttpListener(
    boost::asio::io_context& ioc,
    const toml::table& config)
    : m_io(ioc)
    , m_acceptor(boost::asio::make_strand(ioc))
    , m_handlers(std::make_shared<std::map<std::tuple<std::string, std::string>, std::shared_ptr<HttpRequestHandler>>>())
{
    std::string host = config["http"]["addr"].value<std::string>().value_or("127.0.0.1");
    int port = config["http"]["port"].value<int>().value_or(1337);

    boost::system::error_code ec;
    auto address = boost::asio::ip::make_address(host, ec);

    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to parse HTTP address: " << ec.message() << " - defaulting to 127.0.0.1";
        address = boost::asio::ip::make_address("127.0.0.1");
    }

    auto endpoint = boost::asio::ip::tcp::endpoint { address, static_cast<uint16_t>(port) };

    BOOST_LOG_TRIVIAL(info) << "Running HTTP server on " << endpoint;

    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen(boost::asio::socket_base::max_listen_connections);
}

HttpListener::~HttpListener()
{
    BOOST_LOG_TRIVIAL(info) << "Shutting down HTTP server";
}

void HttpListener::AddHandler(
    const std::string& method,
    const std::string& path,
    const std::shared_ptr<HttpRequestHandler>& handler)
{
    m_handlers->insert({ { method, path }, handler });
}

void HttpListener::Run()
{
    boost::asio::dispatch(
        m_acceptor.get_executor(),
        boost::beast::bind_front_handler(
            &HttpListener::BeginAccept,
            shared_from_this()));
}

void HttpListener::Stop()
{
    for (const auto& session : m_sessions)
    {
        if (auto lock = session.lock())
        {
            lock->Stop();
        }
    }

    m_sessions.clear();
    m_handlers->clear();
    m_acceptor.close();

    BOOST_LOG_TRIVIAL(info) << "HTTP server stopped";
}

void HttpListener::BeginAccept()
{
    m_acceptor.async_accept(
        boost::asio::make_strand(m_io),
        boost::beast::bind_front_handler(
            &HttpListener::EndAccept,
            shared_from_this()));
}

void HttpListener::EndAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
{
    if(ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Error when accepting HTTP client: " << ec.message();
    }
    else
    {
        auto session = std::make_shared<HttpSession>(
            std::move(socket),
            m_handlers);

        m_sessions.push_back(session);

        session->Run();
    }

    BeginAccept();
}
