#include "migrator.hpp"

#include <boost/log/trivial.hpp>

#include "migrations/0001_initialsetup.hpp"
#include "transaction.hpp"

using pika::Data::Migrator;
using pika::Data::Transaction;

static int GetUserVersion(sqlite3* db)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "PRAGMA user_version;", -1, &stmt, nullptr);
    sqlite3_step(stmt);
    int version = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return version;
}

static void SetUserVersion(sqlite3* db, int version)
{
    std::string sql = "PRAGMA user_version = " + std::to_string(version);

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

bool Migrator::Run(sqlite3 *db)
{
    static std::vector<std::function<int(sqlite3*)>> migrations =
    {
        &Migrations::InitialSetup::Migrate
    };

    int userVersion = GetUserVersion(db);

    if (userVersion == migrations.size())
    {
        return true;
    }

    BOOST_LOG_TRIVIAL(info)
        << "Migrating database from version " << userVersion
        << " to version " << migrations.size();

    Transaction tx(db);

    for (int i = userVersion; i < migrations.size(); i++)
    {
        int res = migrations[i](db);

        if (res != SQLITE_OK)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to apply migration: " << sqlite3_errmsg(db);
            tx.Rollback();
            return false;
        }
    }

    SetUserVersion(db, static_cast<int>(migrations.size()));

    tx.Commit();

    return true;
}
