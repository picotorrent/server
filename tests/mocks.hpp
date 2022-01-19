#pragma once

#include <gmock/gmock.h>

#include "../src/session.hpp"
#include "../src/sessionmanager.hpp"

class MockSession : public pt::Server::ISession
{
public:
    MOCK_METHOD(lt::info_hash_t, AddTorrent, (lt::add_torrent_params& params));
    MOCK_METHOD(void, ApplySettings, (const lt::settings_pack& settings));
    MOCK_METHOD(std::shared_ptr<pt::Server::ITorrentHandle>, FindTorrent, (const lt::info_hash_t &hash));
    MOCK_METHOD(void, ForEachTorrent, (std::function<bool(lt::torrent_status const& ts)> const& iteree));
    MOCK_METHOD(void, RemoveTorrent, (lt::info_hash_t const& hash, bool remove_files));

    lt::add_torrent_params* params;
};

class MockSessionManager : public pt::Server::ISessionManager
{
public:
    MOCK_METHOD(std::weak_ptr<pt::Server::ISession>, Get, (int id));
    MOCK_METHOD(std::weak_ptr<pt::Server::ISession>, GetDefault, ());
};

class MockTorrentHandle : public pt::Server::ITorrentHandle
{
public:
    MOCK_METHOD(bool, IsValid, ());
    MOCK_METHOD(void, Pause, ());
    MOCK_METHOD(void, Resume, ());
};
