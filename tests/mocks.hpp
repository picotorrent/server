#pragma once

#include <gmock/gmock.h>

#include "../src/sessionmanager.hpp"

class MockTorrentHandleFinder : public pt::Server::ITorrentHandleFinder
{
public:
    MOCK_METHOD(std::shared_ptr<pt::Server::ITorrentHandleActor>, Find, (const libtorrent::info_hash_t& hash));
};

class MockTorrentHandleActor : public pt::Server::ITorrentHandleActor
{
public:
    MOCK_METHOD(bool, IsValid, ());
    MOCK_METHOD(void, Pause, ());
    MOCK_METHOD(void, Resume, ());
};

