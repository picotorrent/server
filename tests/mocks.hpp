#pragma once

#include <gmock/gmock.h>

#include "../src/session.hpp"
#include "../src/sessioneventhandler.hpp"
#include "../src/torrenthandle.hpp"

class MockSession : public pika::ISession
{
public:
    MOCK_METHOD(void, AddEventHandler, (std::weak_ptr<pika::ISessionEventHandler> handler));
    MOCK_METHOD(libtorrent::info_hash_t, AddTorrent, (const libtorrent::add_torrent_params &params));
    MOCK_METHOD(std::shared_ptr<pika::ITorrentHandle>, FindTorrent, (const libtorrent::info_hash_t& hash));
    MOCK_METHOD(void, ForEachTorrent, (const std::function<void(const libtorrent::torrent_status&)> &cb));
    MOCK_METHOD(void, RemoveTorrent, (libtorrent::info_hash_t const& hash, bool removeFiles));
};

class MockTorrentHandle : public pika::ITorrentHandle
{
public:
    MOCK_METHOD(libtorrent::info_hash_t, InfoHash, ());
    MOCK_METHOD(bool, IsValid, ());
    MOCK_METHOD((std::map<std::string, std::string>&), Labels, ());
    MOCK_METHOD(void, MoveStorage, (const std::string& path));
    MOCK_METHOD(void, Pause, ());
    MOCK_METHOD(void, Resume, ());
    MOCK_METHOD(libtorrent::torrent_status&, Status, ());
};
