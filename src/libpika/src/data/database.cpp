#include <libpika/data/database.hpp>

#include <libpika/data/statement.hpp>
#include <libpika/data/transaction.hpp>
#include <sqlite3.h>

#include "_aux/statement.hpp"
#include "_aux/transaction.hpp"

using libpika::data::Database;

struct Database::State
{
    explicit State(const std::string_view& file)
        : db(nullptr)
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

Database::~Database() = default;

std::unique_ptr<libpika::data::ITransaction> Database::BeginTransaction()
{
    return std::make_unique<libpika::data::_aux::Transaction>(m_state->db);
}

std::unique_ptr<libpika::data::IStatement> Database::PrepareStatement(const std::string_view &sql)
{
    sqlite3_stmt* stmt;
    int res = sqlite3_prepare(m_state->db, sql.data(), -1, &stmt, nullptr);

    if (res != SQLITE_OK)
    {
        throw std::exception();
    }

    return std::make_unique<libpika::data::_aux::Statement>(stmt);
}
