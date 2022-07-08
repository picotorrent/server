#include "eventshandler.hpp"

#include <queue>

#include <boost/log/trivial.hpp>
#include <nlohmann/json.hpp>

#include "../../json/infohash.hpp"
#include "../../json/torrentstatus.hpp"
#include "../../session.hpp"
#include "../../torrenthandle.hpp"

using json = nlohmann::json;
using pika::Http::Handlers::EventsHandler;

class EventsHandler::ContextState : public std::enable_shared_from_this<EventsHandler::ContextState>
{
public:
    explicit ContextState(std::shared_ptr<pika::Http::Context> context)
        : m_ctx(std::move(context))
    {
    }

    bool IsDead() const { return m_dead; }

    int64_t SentBytes() const { return m_sent; }

    void QueueWrite(std::string data)
    {
        if (m_dead) { return; }
        m_sendData.push(std::move(data));
        MaybeWrite();
    }

private:
    void MaybeWrite()
    {
        if (m_dead
            || m_isWriting
            || m_sendData.empty())
        {
            return;
        }

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

            if (ec == boost::asio::error::broken_pipe
                || ec == boost::asio::error::timed_out)
            {
                // Client disconnected
                return;
            }

            BOOST_LOG_TRIVIAL(error)
                << "Failed to write " << bytes
                << " bytes: " << ec.message()
                << " (" << ec.value() << ")";

            return;
        }

        m_sent += bytes;

        MaybeWrite();
    }

    bool m_dead {false};
    bool m_isWriting {false};
    int64_t m_sent{0};
    std::queue<std::string> m_sendData;
    std::shared_ptr<pika::Http::Context> m_ctx;
};

EventsHandler::EventsHandler(boost::asio::io_context &io, std::weak_ptr<pika::ISession> session)
    : m_heartbeat(io)
    , m_session(std::move(session))
{
    m_heartbeat.expires_after(std::chrono::seconds(5));
    m_heartbeat.async_wait(
        [this](auto &&PH1)
        {
            OnHeartbeatExpired(std::forward<decltype(PH1)>(PH1));
        });
}

void EventsHandler::operator()(std::shared_ptr<Http::Context> context)
{
    if (context->Request().target().ends_with("?stats"))
    {
        json::array_t j;

        for (const auto& ctx : m_ctxs)
        {
            j.push_back({
                {"is_dead", ctx->IsDead()},
                {"sent",    ctx->SentBytes()}
            });
        }

        context->WriteJson(j);

        return;
    }

    std::string headers = "HTTP/1.1 200 OK\n"
                          "Connection: keep-alive\n"
                          "Content-Type: text/event-stream\n"
                          "Cache-Control: no-cache, no-transform\n\n";

    if (auto session = m_session.lock())
    {
        json::array_t torrents;
        session->ForEachTorrent(
            [&torrents](const auto &ts)
            {
                torrents.push_back({
                                       {"info_hash", ts.info_hashes}
                                   });
            });

        json initial = {
            {"torrents", torrents}
        };

        std::stringstream evt;
        evt << "event: initial_state\n";
        evt << "data: " << initial.dump() << "\n\n";

        auto state = std::make_shared<ContextState>(std::move(context));
        state->QueueWrite(headers);
        state->QueueWrite(evt.str());

        m_ctxs.push_back(state);
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Could not get session lock";
    }
}

void EventsHandler::OnSessionStats(const std::map<std::string, int64_t> &stats)
{
    // Move fetching of metrics to a JSONRPC method
    Broadcast("session_metrics_updated", "{}");
}

void EventsHandler::OnStateUpdate(const std::vector<std::shared_ptr<ITorrentHandle>> &torrents)
{
    json state;

    for (const auto& torrent : torrents)
    {
        json j = {
            {"info_hash", torrent->InfoHash()}
        };

        state.push_back(j);
    }

    Broadcast("state_update", state.dump());
}

void EventsHandler::OnTorrentAdded(const std::shared_ptr<ITorrentHandle> &handle)
{
    json j = {
        {"info_hash", handle->InfoHash()}
    };

    Broadcast("torrent_added", j.dump());
}

void EventsHandler::OnTorrentPaused(const lt::info_hash_t &hash)
{
    json j = {
        {"info_hash", hash}
    };

    Broadcast("torrent_paused", j.dump());
}

void EventsHandler::OnTorrentRemoved(const lt::info_hash_t &hash)
{
    json j = {
        {"info_hash", hash}
    };

    Broadcast("torrent_removed", j.dump());
}

void EventsHandler::OnTorrentResumed(const lt::info_hash_t &hash)
{
    json j = {
        {"info_hash", hash}
    };

    Broadcast("torrent_resumed", j.dump());
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
