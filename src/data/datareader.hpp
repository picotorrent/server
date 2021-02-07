#pragma once

#include <optional>
#include <string>

#include <sqlite3.h>

static std::optional<std::string> pt_sqlite3_column_std_string(sqlite3_stmt* stmt, int columnId)
{
    auto data = sqlite3_column_text(stmt, columnId);
    if (data == nullptr) { return std::nullopt; }
    return std::string(
        reinterpret_cast<const char*>(data),
        sqlite3_column_bytes(stmt, columnId));
}
