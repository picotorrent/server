#pragma once

#include <libtorrent/info_hash.hpp>

namespace pika
{
    class ITorrentHandle
    {
    public:
        virtual ~ITorrentHandle() = default;

        virtual libtorrent::info_hash_t InfoHash() = 0;
        virtual bool IsValid() = 0;
        virtual void MoveStorage(const std::string& path) = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
    };
}
