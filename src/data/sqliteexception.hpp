#pragma once

#include <exception>

namespace pika::Data
{
    class SQLiteException : public std::exception
    {
    };
}
