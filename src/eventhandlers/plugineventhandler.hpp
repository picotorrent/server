#pragma once

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

#include "../sessioneventhandler.hpp"

namespace pika::EventHandlers
{
    class PluginEventHandler : public ISessionEventHandler
    {
    public:
        explicit PluginEventHandler(boost::asio::io_context &io);

        void OnSessionStats(const std::map<std::string, int64_t> &) override;
        void OnTorrentAdded() override;

    private:
        void Send(const std::string_view &url, const nlohmann::json &data);

        boost::asio::io_context &m_io;
    };
}
