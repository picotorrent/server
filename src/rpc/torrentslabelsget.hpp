#pragma once

#include <libpika/jsonrpc/method.hpp>
#include <sqlite3.h>

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsLabelsGetCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit TorrentsLabelsGetCommand(sqlite3* db, ISession&);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
        ISession& m_session;
    };
}
