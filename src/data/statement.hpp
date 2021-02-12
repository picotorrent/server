#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace pt::Server::Data
{
    class Statement
    {
    public:
        struct Row
        {
            friend class Statement;

            bool        GetBool(int col)      const;
            int         GetInt32(int col)     const;
            std::string GetStdString(int col) const;
            bool        IsNull(int col)       const;

        private:
            Row(sqlite3_stmt* stmt);
            sqlite3_stmt* m_stmt;
        };

        static void ForEach(
            sqlite3* db,
            std::string const& sql,
            std::function<void(Row const&)> const& cb,
            std::function<void(sqlite3_stmt*)> bind = {});
    };
}
