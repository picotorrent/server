#include "row.hpp"

using libpika::data::_aux::Row;

Row::Row(sqlite3_stmt *stmt)
    : m_stmt(stmt)
{}

std::vector<char> Row::GetBuffer(int index)
{
    int len = sqlite3_column_bytes(m_stmt, index);
    const char* buf = static_cast<const char*>(sqlite3_column_blob(m_stmt, index));
    return {buf, buf + len};
}

int Row::GetInt32(int index)
{
    return sqlite3_column_int(m_stmt, index);
}

std::string Row::GetStdString(int index)
{
    auto data = sqlite3_column_text(m_stmt, index);
    return std::string(
            reinterpret_cast<const char*>(data),
            sqlite3_column_bytes(m_stmt, index));
}
