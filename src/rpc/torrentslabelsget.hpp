#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsLabelsGetCommand : public Command
    {
    public:
        TorrentsLabelsGetCommand(sqlite3* db, ISession&);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
        ISession& m_session;
    };
}
