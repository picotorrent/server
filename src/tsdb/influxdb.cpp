#include "influxdb.hpp"

#include <functional>
#include <queue>

#include <boost/beast.hpp>
#include <boost/log/trivial.hpp>

using pt::Server::TSDB::InfluxDb;

class InfluxDb::HttpClient : public std::enable_shared_from_this<HttpClient>
{
public:
    explicit HttpClient(
        boost::asio::io_context& io,
        std::string host,
        int16_t port,
        std::string org,
        std::string bucket,
        std::string token)
        : m_dbHost(std::move(host)),
          m_dbPort(port),
          m_dbOrg(std::move(org)),
          m_dbBucket(std::move(bucket)),
          m_dbToken(std::move(token)),
          m_resolver(boost::asio::make_strand(io)),
          m_stream(boost::asio::make_strand(io))
    {
    }

    void Post(std::string const& data, std::function<void(boost::system::error_code)> const& cb)
    {
        m_body = data;
        m_callback = cb;

        std::stringstream target;
        target << "/api/v2/write?bucket=" << m_dbBucket << "&org=" << m_dbOrg << "&precision=ms";

        m_req.version(11);
        m_req.method(boost::beast::http::verb::post);
        m_req.target(target.str());
        m_req.set(boost::beast::http::field::host, m_dbHost);
        m_req.set("Authorization", "Token " + m_dbToken);
        m_req.set("Content-Length", std::to_string(m_body.size()));
        m_req.set("Content-Type", "text/plain");

        m_req.body() = m_body;

        m_resolver.async_resolve(
            m_dbHost,
            std::to_string(m_dbPort),
            boost::beast::bind_front_handler(
                &HttpClient::OnAsyncTcpResolve,
                shared_from_this()));
    }

private:
    void OnAsyncHttpRead(boost::system::error_code ec, std::size_t bytes)
    {
        if (ec)
        {
            m_callback(ec);
            return;
        }

        BOOST_LOG_TRIVIAL(debug) << "HttpClient response read: " << bytes;

        m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);

        m_callback({});
    }

    void OnAsyncHttpWrite(boost::system::error_code ec, std::size_t bytes)
    {
        if (ec)
        {
            m_callback(ec);
            return;
        }

        BOOST_LOG_TRIVIAL(debug) << "HttpClient request data written: " << bytes;

        boost::beast::http::async_read(
            m_stream,
            m_buffer,
            m_res,
            boost::beast::bind_front_handler(
                &HttpClient::OnAsyncHttpRead,
                shared_from_this()));
    }

    void OnAsyncStreamConnect(boost::system::error_code ec, const boost::asio::ip::tcp::resolver::endpoint_type&)
    {
        if (ec)
        {
            m_callback(ec);
            return;
        }

        m_stream.expires_after(std::chrono::seconds(30));

        boost::beast::http::async_write(
            m_stream,
            m_req,
            boost::beast::bind_front_handler(
                &HttpClient::OnAsyncHttpWrite,
                shared_from_this()));
    }

    void OnAsyncTcpResolve(boost::system::error_code ec, const boost::asio::ip::tcp::resolver::results_type& results)
    {
        if (ec)
        {
            m_callback(ec);
            return;
        }

        m_stream.async_connect(
            results,
            boost::beast::bind_front_handler(
                &HttpClient::OnAsyncStreamConnect,
                shared_from_this()));
    }

    std::string m_dbHost;
    int16_t m_dbPort;
    std::string m_dbOrg;
    std::string m_dbBucket;
    std::string m_dbToken;

    boost::asio::ip::tcp::resolver m_resolver;
    boost::beast::tcp_stream m_stream;
    boost::beast::http::request<boost::beast::http::string_body> m_req;
    boost::beast::http::response<boost::beast::http::string_body> m_res;

    std::function<void(boost::system::error_code)> m_callback;
    std::string m_body;
    boost::beast::flat_buffer m_buffer;
};

InfluxDb::InfluxDb(
    boost::asio::io_context &io,
    const std::string& host,
    int16_t port,
    const std::string& org,
    const std::string& bucket,
    const std::string& token)
    : m_io(io),
      m_timer(io),
      m_httpClient(
          std::make_shared<HttpClient>(
              io,
              host,
              port,
              org,
              bucket,
              token))
{
    m_timer.expires_from_now(boost::posix_time::seconds(5));
    m_timer.async_wait([this](auto && PH1) { Send(std::forward<decltype(PH1)>(PH1)); });
}

InfluxDb::~InfluxDb() = default;

void InfluxDb::WriteMetrics(
    const std::map<std::string, int64_t> &metrics,
    std::chrono::milliseconds timestamp)
{
    m_current.push_back(Metrics{ .data = metrics, .timestamp = timestamp });
}

void InfluxDb::Send(boost::system::error_code ec)
{
    if (ec)
    {
        BOOST_LOG_TRIVIAL(error) << "Error when timer expired: " << ec;
        return;
    }

    // Move all items from current to in-flight

    m_inflight.insert(
        m_inflight.begin(),
        std::make_move_iterator(m_current.begin()),
        std::make_move_iterator(m_current.end()));

    m_current.erase(
        m_current.begin(),
        m_current.end());

    BOOST_LOG_TRIVIAL(debug) << "Sending " << m_inflight.size() << " measurements to InfluxDb";

    std::stringstream measurements;

    for (auto const& item : m_inflight)
    {
        measurements << "session ";

        for (auto const& metric : item.data)
        {
            measurements << metric.first << "=" << metric.second << ",";
        }

        measurements << "f_f=0 ";
        measurements << item.timestamp.count() << "\n";
    }

    m_httpClient->Post(
        measurements.str(),
        [this](boost::system::error_code ec)
        {
            if (ec)
            {
                // TODO: in this case, we should not clear the in-flight list
                //       but instead retry

                BOOST_LOG_TRIVIAL(error) << "Failed to POST data to InfluxDB: " << ec.message();
            }

            m_inflight.erase(
                m_inflight.begin(),
                m_inflight.end());

            m_timer.expires_from_now(boost::posix_time::seconds(5));
            m_timer.async_wait([this](auto && PH1) { Send(std::forward<decltype(PH1)>(PH1)); });
        });
}
