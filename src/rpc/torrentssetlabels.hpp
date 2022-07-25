#pragma once

#include <libpika/jsonrpc/method.hpp>
#include <sqlite3.h>

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsLabelsSetCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsLabelsSetCommand(sqlite3* db, ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
        ISession& m_session;
    };
}
