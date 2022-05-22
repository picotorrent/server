#pragma once

namespace pika
{
    class ITorrentHandle
    {
    public:
        virtual ~ITorrentHandle() = default;

        virtual bool IsValid() = 0;
        virtual void MoveStorage(const std::string& path) = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
    };
}
