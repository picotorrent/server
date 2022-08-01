#pragma once

#include <libpika/data/row.hpp>
#include <sqlite3.h>

namespace libpika::data::_aux
{
    class Row : public libpika::data::IRow
    {
    public:
        explicit Row(sqlite3_stmt* stmt);

        std::vector<char> GetBuffer(int index) override;
        int GetInt32(int index) override;
        std::string GetStdString(int index) override;

    private:
        sqlite3_stmt* m_stmt;
    };
}
