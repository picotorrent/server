#include "plugineventhandler.hpp"

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

#include "../http/httpclient.hpp"

using json = nlohmann::json;
using pika::EventHandlers::PluginEventHandler;

PluginEventHandler::PluginEventHandler(boost::asio::io_context &io)
    : m_io(io)
{
}

void PluginEventHandler::OnSessionStats(const std::map<std::string, int64_t> &stats)
{
    Send("http://localhost:3000/events", {});
}

void PluginEventHandler::OnTorrentAdded()
{
    // For each plugin, post data
    json data =
    {
        { "foo", "bar "}
    };

    Send("http://localhost:3000/events", data);
}

void PluginEventHandler::Send(const std::string_view &url, const nlohmann::json &data)
{
    std::make_shared<Http::HttpClient>(m_io)
        ->Send(url);
}
