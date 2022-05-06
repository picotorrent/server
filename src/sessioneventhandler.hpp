#pragma once

#include <map>

#include <libtorrent/info_hash.hpp>

namespace pika
{
    class ITorrentHandle;

    class ISessionEventHandler
    {
    public:
        virtual ~ISessionEventHandler() = default;

        virtual void OnSessionStats(const std::map<std::string, int64_t> &stats) {}
        virtual void OnStateUpdate(const std::vector<std::shared_ptr<ITorrentHandle>> &) {}
        virtual void OnTorrentAdded(const std::shared_ptr<ITorrentHandle> &) {}
        virtual void OnTorrentRemoved(const libtorrent::info_hash_t &) {}
    };
}
