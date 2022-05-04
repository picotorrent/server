#pragma once

#include <map>

namespace pika
{
    class ISessionEventHandler
    {
    public:
        virtual ~ISessionEventHandler() = default;

        virtual void OnSessionStats(const std::map<std::string, int64_t> &stats) {}
        virtual void OnStateUpdate() {}
        virtual void OnTorrentAdded() {}
        virtual void OnTorrentRemoved() {}
    };
}
