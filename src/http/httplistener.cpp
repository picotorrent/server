#include "httplistener.hpp"

#include <boost/log/trivial.hpp>

#include "httpsession.hpp"
#include "../sessionmanager.hpp"

using pt::Server::SessionManager;
using pt::Server::Http::HttpListener;

HttpListener::HttpListener(
    boost::asio::io_context& ioc,
    boost::asio::ip::tcp::endpoint endpoint,
    sqlite3* db,
    std::shared_ptr<SessionManager> const& session,
    std::shared_ptr<std::string const> const& docroot)
    : m_io(ioc)
    , m_acceptor(boost::asio::make_strand(ioc))
    , m_db(db)
    , m_session(session)
    , m_docroot(docroot)
    , m_commands(std::make_shared<std::map<std::string, std::shared_ptr<pt::Server::RPC::Command>>>())
{
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen(boost::asio::socket_base::max_listen_connections);
}

std::map<std::string, std::shared_ptr<pt::Server::RPC::Command>>& HttpListener::Commands()
{
    return *m_commands;
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
            m_db,
            m_session,
            m_docroot,
            m_commands)->Run();
    }

    BeginAccept();
}
