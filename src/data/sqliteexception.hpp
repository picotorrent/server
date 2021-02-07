#pragma once

#include <exception>

namespace pt::Server::Data
{
    class SQLiteException : public std::exception
    {
    };
}
