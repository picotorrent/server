#include "statement.hpp"

#include <libpika/data/row.hpp>

#include "row.hpp"

using libpika::data::_aux::Statement;

Statement::Statement(sqlite3_stmt *stmt)
    : m_stmt(stmt)
{
}

Statement::~Statement()
{
    sqlite3_finalize(m_stmt);
}

void Statement::Bind(int pos, int value)
{
    sqlite3_bind_int(m_stmt, pos, value);
}

void Statement::Bind(int pos, const std::vector<char> &value)
{
}

void Statement::Bind(int pos, const std::optional<std::string_view> &value)
{
}

void Statement::Reset()
{
    sqlite3_reset(m_stmt);
}

std::unique_ptr<libpika::data::IRow> Statement::Step()
{
    switch (sqlite3_step(m_stmt))
    {
    case SQLITE_ROW:
        return std::make_unique<libpika::data::_aux::Row>(m_stmt);
    }

    return nullptr;
}
