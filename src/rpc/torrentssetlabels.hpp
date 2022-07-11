#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsLabelsSetCommand : public Command
    {
    public:
        TorrentsLabelsSetCommand(sqlite3* db, ISession& session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
        ISession& m_session;
    };
}
