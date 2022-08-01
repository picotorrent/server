#include <libpika/data/database.hpp>

#include <sqlite3.h>

using libpika::data::Database;

struct Database::State
{
    explicit State(const std::string_view& file)
    {
        sqlite3_open(file.data(), &db);
    }

    ~State()
    {
        sqlite3_close(db);
    }

    sqlite3* db;
};

Database::Database(const std::string_view &file)
{
    m_state = std::make_unique<State>(file);
    sqlite3_exec(m_state->db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
}
