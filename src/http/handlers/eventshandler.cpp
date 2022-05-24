#include "eventshandler.hpp"

#include <queue>

#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>

#include "../../torrenthandle.hpp"

using json = nlohmann::json;
using pika::Http::Handlers::EventsHandler;

class EventsHandler::ContextState : public std::enable_shared_from_this<EventsHandler::ContextState>
{
public:
    explicit ContextState(std::shared_ptr<pika::Http::HttpRequestHandler::Context> context)
        : m_ctx(std::move(context))
    {
    }

    bool IsDead() const { return m_dead; }

    void QueueWrite(std::string data)
    {
        if (m_dead) { return; }
        m_sendData.push(std::move(data));
        MaybeWrite();
    }

private:
    void MaybeWrite()
    {
        if (m_isWriting || m_sendData.empty()) { return; }

        const std::string& data = m_sendData.front();

        boost::asio::async_write(
            m_ctx->Stream(),
            boost::asio::buffer(data),
            [_this = shared_from_this()](boost::system::error_code ec, int b)
            {
                _this->OnWrite(ec, b);
            });

        m_isWriting = true;
    }

    void OnWrite(boost::system::error_code ec, int bytes)
    {
        m_isWriting = false;
        m_sendData.pop();

        if (ec)
        {
            m_dead = true;

            if (ec == boost::asio::error::broken_pipe)
            {
                // Client disconnected
                return;
            }

            BOOST_LOG_TRIVIAL(error) << "Failed to write " << bytes << " bytes: " << ec.message();
            return;
        }

        MaybeWrite();
    }

    bool m_dead {false};
    bool m_isWriting {false};
    std::queue<std::string> m_sendData;
    std::shared_ptr<pika::Http::HttpRequestHandler::Context> m_ctx;
};

EventsHandler::EventsHandler(boost::asio::io_context &io)
    : m_heartbeat(io)
{
    m_heartbeat.expires_after(std::chrono::seconds(5));
    m_heartbeat.async_wait(
        [this](auto &&PH1)
        {
            OnHeartbeatExpired(std::forward<decltype(PH1)>(PH1));
        });
}

void EventsHandler::Execute(std::shared_ptr<HttpRequestHandler::Context> context)
{
    std::string headers = "HTTP/1.1 200 OK\n"
                          "Connection: keep-alive\n"
                          "Content-Type: text/event-stream\n"
                          "Cache-Control: no-cache, no-transform\n\n";

    auto state = std::make_shared<ContextState>(std::move(context));
    state->QueueWrite(headers);

    m_ctxs.push_back(state);
}

void EventsHandler::OnSessionStats(const std::map<std::string, int64_t> &stats)
{
    Broadcast("session_stats", json(stats).dump());
}

void EventsHandler::OnTorrentAdded(const std::shared_ptr<ITorrentHandle> &handle)
{
    json j;
    j["info_hash_v1"] = handle->InfoHash().v1;
    j["info_hash_v2"] = handle->InfoHash().v2;

    Broadcast("torrent_added", j.dump());
}

void EventsHandler::OnTorrentRemoved(const lt::info_hash_t &hash)
{
    json j;
    j["info_hash_v1"] = hash.v1;
    j["info_hash_v2"] = hash.v2;

    Broadcast("torrent_removed", j.dump());
}

void EventsHandler::Broadcast(const std::string& name, const std::string& data)
{
    std::stringstream evt;
    evt << "event: "<< name << "\n";
    evt << "data: " << data << "\n\n";

    m_ctxs.erase(
        std::remove_if(
            m_ctxs.begin(),
            m_ctxs.end(),
            [](auto& ptr) { return ptr->IsDead(); }),
        m_ctxs.end());

    for (auto& ctx : m_ctxs)
    {
        ctx->QueueWrite(evt.str());
    }
}

void EventsHandler::OnHeartbeatExpired(boost::system::error_code ec)
{
    m_heartbeat.expires_after(std::chrono::seconds(5));
    m_heartbeat.async_wait(
        [this](auto &&PH1)
        {
            OnHeartbeatExpired(std::forward<decltype(PH1)>(PH1));
        });

    Broadcast("heartbeat", "{}");
}
