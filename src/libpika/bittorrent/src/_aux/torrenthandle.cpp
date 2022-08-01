#include "torrenthandle.hpp"

#include <boost/log/trivial.hpp>

using libpika::bittorrent::_aux::TorrentHandle;

TorrentHandle::TorrentHandle(lt::torrent_status status, std::map<std::string, std::string>& labels)
    : m_status(std::move(status))
    , m_labels(labels)
{
}

lt::info_hash_t TorrentHandle::InfoHash()
{
    return m_status.info_hashes;
}

bool TorrentHandle::IsValid()
{
    return m_status.handle.is_valid();
}

std::map<std::string, std::string>& TorrentHandle::Labels()
{
    return m_labels;
}

void TorrentHandle::MoveStorage(const std::string& path)
{
    BOOST_LOG_TRIVIAL(info) << "Moving " << m_status.name << " to " << path;
    m_status.handle.move_storage(path);
}

void TorrentHandle::Pause()
{
    m_status.handle.unset_flags(
        lt::torrent_flags::auto_managed);

    m_status.handle.pause();
}

void TorrentHandle::Resume()
{
    m_status.handle.set_flags(
        lt::torrent_flags::auto_managed);

    m_status.handle.resume();
}

const lt::torrent_status& TorrentHandle::Status()
{
    return m_status;
}
