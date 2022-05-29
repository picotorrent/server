#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsSetLabelsCommand : public Command
    {
    public:
        TorrentsSetLabelsCommand(sqlite3* db, std::weak_ptr<ISession> session);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
        std::weak_ptr<ISession> m_session;
    };
}
