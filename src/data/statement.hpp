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

            [[nodiscard]] std::vector<char> GetBlob(int col)                     const;
            [[nodiscard]] bool              GetBool(int col)                     const;
            [[nodiscard]] bool              GetBool(const std::string& col)      const;
            [[nodiscard]] int               GetInt32(int col)                    const;
            [[nodiscard]] int               GetInt32(const std::string& col)     const;
            [[nodiscard]] std::string       GetStdString(int col)                const;
            [[nodiscard]] std::string       GetStdString(const std::string& col) const;
            [[nodiscard]] bool              IsNull(int col)                      const;

        private:
            Row(sqlite3_stmt* stmt, const std::map<std::string, int>& cols);
            sqlite3_stmt* m_stmt;
            const std::map<std::string, int>& m_cols;
        };

        static void ForEach(
            sqlite3* db,
            std::string const& sql,
            std::function<void(Row const&)> const& cb,
            std::function<void(sqlite3_stmt*)> bind = {});
    };
}
