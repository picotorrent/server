#include "database.hpp"

#include <functional>
#include <map>
#include <string>

#include <boost/log/trivial.hpp>

#include "data/migrations/0001_initialsetup.hpp"
#include "data/migrations/0002_simplify.hpp"
#include "data/transaction.hpp"

using pt::Server::Database;

static std::map<std::string, std::function<int(sqlite3*)>> DatabaseMigrations =
{
    { "0001_InitialSetup", &pt::Server::Data::Migrations::InitialSetup::Migrate },
    { "0002_Simplify", &pt::Server::Data::Migrations::Simplify::Migrate }
};

bool CreateMigrationsTable(sqlite3* db)
{
    int res = sqlite3_exec(
        db,
        "CREATE TABLE IF NOT EXISTS migrations (id TEXT PRIMARY KEY);",
        nullptr,
        nullptr,
        nullptr);

    return res == SQLITE_OK;
}

bool MigrationExists(sqlite3* db, std::string const& id)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM migrations WHERE id = $1", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, id.c_str(), static_cast<int>(id.size()), SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    int cnt = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return cnt >= 1;
}

bool Database::Migrate(sqlite3* db)
{
    Data::Transaction tx(db);

    BOOST_LOG_TRIVIAL(debug) << "Creating migrations table";

    if (!CreateMigrationsTable(db))
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to create migrations table";
        tx.Rollback();
        return false;
    }

    // Run migrations
    for (auto const& migration : DatabaseMigrations)
    {
        if (MigrationExists(db, migration.first))
        {
            BOOST_LOG_TRIVIAL(debug) << "Migration '" << migration.first << "' exists - skipping...";
            continue;
        }

        int res = migration.second(db);

        if (res != SQLITE_OK)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to apply migration: " << sqlite3_errmsg(db);
            tx.Rollback();
            return false;
        }

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "INSERT INTO migrations (id) VALUES ($1);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, migration.first.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        BOOST_LOG_TRIVIAL(info) << "Applied migration " << migration.first;
    }

    tx.Commit();

    return true;
}
