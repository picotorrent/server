#pragma once

#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <sqlite3.h>

namespace pt::Server
{
    class ITorrentHandleActor
    {
    public:
        virtual ~ITorrentHandleActor() = default;

        virtual bool IsValid() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;
    };

    class ISession;

    class ISessionManager
    {
    public:
        virtual ~ISessionManager() {}

        virtual std::weak_ptr<ISession> Get(int sessionId) = 0;
        virtual std::weak_ptr<ISession> GetDefault() = 0;
    };

    class SessionManager : public ISessionManager
    {
    public:
        static std::shared_ptr<ISessionManager> Load(boost::asio::io_context& io, sqlite3* db);

        ~SessionManager() override;
        std::weak_ptr<ISession> Get(int sessionId) override;
        std::weak_ptr<ISession> GetDefault() override;

    private:
        explicit SessionManager(std::vector<std::shared_ptr<ISession>> sessions);

        std::vector<std::shared_ptr<ISession>> m_sessions;
    };
}
