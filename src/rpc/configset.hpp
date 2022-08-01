#pragma once

#include <libpika/jsonrpc/method.hpp>

namespace libpika::data
{
    class Database;
}

namespace pika::RPC
{
    class ConfigSetCommand : public libpika::jsonrpc::Method
    {
    public:
        explicit ConfigSetCommand(libpika::data::Database& db);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        libpika::data::Database& m_db;
    };
}
