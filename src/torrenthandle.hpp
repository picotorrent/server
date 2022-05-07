#pragma once

namespace pika
{
    class ITorrentHandle
    {
    public:
        virtual ~ITorrentHandle() = default;

        virtual bool IsValid() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
    };
}
