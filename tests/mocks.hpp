#pragma once

#include <gmock/gmock.h>

#include "../src/session.hpp"

class MockSession : public pt::Server::ISession
{
public:
    MOCK_METHOD(std::shared_ptr<pt::Server::ITorrentHandle>, FindTorrent, (const libtorrent::info_hash_t& hash));
};

class MockTorrentHandle : public pt::Server::ITorrentHandle
{
public:
    MOCK_METHOD(bool, IsValid, ());
    MOCK_METHOD(void, Pause, ());
    MOCK_METHOD(void, Resume, ());
};
