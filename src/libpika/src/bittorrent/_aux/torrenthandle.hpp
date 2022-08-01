#pragma once

#include <libpika/bittorrent/torrenthandle.hpp>

namespace libpika::bittorrent::_aux
{
    class TorrentHandle : public libpika::bittorrent::ITorrentHandle
    {
    public:
        explicit TorrentHandle(lt::torrent_status status, std::map<std::string, std::string>& labels);

        ~TorrentHandle() override = default;

        lt::info_hash_t InfoHash() override;
        bool IsValid() override;
        std::map<std::string, std::string>& Labels() override;
        void MoveStorage(const std::string& path) override;
        void Pause() override;
        void Resume() override;
        const lt::torrent_status& Status() override;

    private:
        lt::torrent_status m_status;
        std::map<std::string, std::string>& m_labels;
    };
}
