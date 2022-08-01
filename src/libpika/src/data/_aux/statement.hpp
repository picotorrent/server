#pragma once

#include <libpika/data/statement.hpp>
#include <sqlite3.h>

namespace libpika::data::_aux
{
    class Statement : public libpika::data::IStatement
    {
    public:
        explicit Statement(sqlite3_stmt* stmt);
        ~Statement() override;

        void Bind(int pos, int value) override;
        void Bind(int pos, const std::optional<std::string_view>& value) override;
        void Bind(int pos, const std::vector<char>& value) override;

        void Reset() override;
        std::unique_ptr<IRow> Step() override;

    private:
        sqlite3_stmt* m_stmt;
    };
}
