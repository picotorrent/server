#pragma once

#include <gmock/gmock.h>

#include "../src/session.hpp"

class MockSession : public pika::ISession
{
public:
    MOCK_METHOD(std::shared_ptr<pika::ITorrentHandle>, FindTorrent, (const libtorrent::info_hash_t& hash));
};

class MockTorrentHandle : public pika::ITorrentHandle
{
public:
    MOCK_METHOD(bool, IsValid, ());
    MOCK_METHOD(void, Pause, ());
    MOCK_METHOD(void, Resume, ());
};
