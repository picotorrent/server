#pragma once

#include <libpika/jsonrpc/method.hpp>
#include <sqlite3.h>

namespace pika::RPC
{
    class ConfigSetCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit ConfigSetCommand(sqlite3* db);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        sqlite3* m_db;
    };
}
