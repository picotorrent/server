#pragma once

#include <exception>
#include <sqlite3.h>

namespace pika::Data
{
    class SQLiteException : public std::exception
    {
    public:
        explicit SQLiteException(sqlite3* db)
            : m_msg(sqlite3_errmsg(db))
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return m_msg; }

    private:
        const char* m_msg;
    };
}
