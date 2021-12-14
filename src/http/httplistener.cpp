#include "httplistener.hpp"

#include <boost/log/trivial.hpp>
#include <utility>

#include "httpsession.hpp"

using pt::Server::Http::HttpListener;
using pt::Server::Http::HttpRequestHandler;

HttpListener::HttpListener(
    boost::asio::io_context& ioc,
    const boost::asio::ip::tcp::endpoint& endpoint,
    std::shared_ptr<std::string const>  docroot)
    : m_io(ioc)
    , m_acceptor(boost::asio::make_strand(ioc))
    , m_docroot(std::move(docroot))
    , m_handlers(std::make_shared<std::map<std::tuple<std::string, std::string>, std::shared_ptr<HttpRequestHandler>>>())
{
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen(boost::asio::socket_base::max_listen_connections);
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
        std::make_shared<HttpSession>(
            std::move(socket),
            m_handlers,
            m_docroot)->Run();
    }

    BeginAccept();
}
