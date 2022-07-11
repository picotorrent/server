#include <libpika/http/server.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/log/trivial.hpp>

#include <libpika/http/context.hpp>
#include <libpika/http/httpsession.hpp>

using libpika::http::HttpServer;

class HttpServer::State : public std::enable_shared_from_this<HttpServer::State>
{
public:
    State(boost::asio::io_context& io, const std::string& host, uint16_t port)
        : m_io(io)
        , m_acceptor(boost::asio::make_strand(io))
    {
        boost::system::error_code ec;
        auto address = boost::asio::ip::make_address(host, ec);

        if (ec)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to parse HTTP address: " << ec.message() << " - defaulting to 127.0.0.1";
            address = boost::asio::ip::make_address("127.0.0.1");
        }

        auto endpoint = boost::asio::ip::tcp::endpoint { address, static_cast<uint16_t>(port) };

        BOOST_LOG_TRIVIAL(info) << "Running HTTP server on " << endpoint;

        m_acceptor.open(endpoint.protocol(), ec);
        if (ec) { BOOST_LOG_TRIVIAL(error) << ec; }

        m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) { BOOST_LOG_TRIVIAL(error) << ec; }

        m_acceptor.bind(endpoint, ec);
        if (ec) { BOOST_LOG_TRIVIAL(error) << ec; }

        m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) { BOOST_LOG_TRIVIAL(error) << ec; }
    }

    ~State() = default;

    void Start()
    {
        boost::asio::dispatch(
            m_acceptor.get_executor(),
            boost::beast::bind_front_handler(
                &State::BeginAccept,
                shared_from_this()));
    }

    void Stop()
    {
        m_acceptor.cancel();
        m_middlewares.clear();

        for (auto&& s : m_sessions)
        {
            if (auto sl = s.lock())
            {
                sl->Stop();
            }
        }

        m_sessions.clear();
    }

    void Use(const Middleware& middleware)
    {
        m_middlewares.emplace_back(middleware);
    }

private:
    void BeginAccept()
    {
        m_acceptor.async_accept(
            boost::asio::make_strand(m_io),
            boost::beast::bind_front_handler(
                &State::EndAccept,
                shared_from_this()));
    }

    void EndAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
    {
        if(ec)
        {
            BOOST_LOG_TRIVIAL(error) << "Error when accepting HTTP client: " << ec.message();
        }
        else
        {
            auto session = std::make_shared<HttpSession>(
                std::move(socket),
                m_middlewares);

            m_sessions.push_back(session);

            session->Run();
        }

        BeginAccept();
    }

    boost::asio::io_context& m_io;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::vector<std::weak_ptr<HttpSession>> m_sessions;

    std::vector<Middleware> m_middlewares;
};

HttpServer::HttpServer(boost::asio::io_context& io, const Options &options)
{
    m_state = std::make_shared<State>(io, options.host, options.port);
    m_state->Start();
}

HttpServer::~HttpServer()
{
    m_state->Stop();
}

void HttpServer::Use(const Middleware& middleware)
{
    m_state->Use(middleware);
}
