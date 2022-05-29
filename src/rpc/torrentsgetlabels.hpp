#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

#include "command.hpp"

namespace pika { class ISession; }

namespace pika::RPC
{
    class TorrentsGetLabelsCommand : public Command
    {
    public:
        TorrentsGetLabelsCommand(sqlite3* db, std::weak_ptr<ISession>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
        std::weak_ptr<ISession> m_session;
    };
}
