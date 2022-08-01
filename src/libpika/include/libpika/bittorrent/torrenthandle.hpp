#pragma once

#include <map>

#include <libtorrent/info_hash.hpp>
#include <libtorrent/torrent_status.hpp>

namespace libpika::bittorrent
{
    class ITorrentHandle
    {
    public:
        virtual ~ITorrentHandle() = default;

        virtual libtorrent::info_hash_t InfoHash() = 0;
        virtual bool IsValid() = 0;
        virtual std::map<std::string, std::string>& Labels() = 0;
        virtual void MoveStorage(const std::string& path) = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
        virtual const libtorrent::torrent_status& Status() = 0;
    };
}
