#include <libpika/data/migrator.hpp>

#include <boost/log/trivial.hpp>
#include <libpika/data/database.hpp>
#include <libpika/data/row.hpp>
#include <libpika/data/statement.hpp>
#include <libpika/data/transaction.hpp>

using libpika::data::Migrator;

static int GetUserVersion(libpika::data::Database& db)
{
    return db.PrepareStatement("PRAGMA user_version;")
        ->Step()
        ->GetInt32(0);
}

static void SetUserVersion(libpika::data::Database& db, int version)
{
    std::string sql = "PRAGMA user_version = " + std::to_string(version);
    db.PrepareStatement(sql)
        ->Step();
}

bool Migrator::Run(Database& db, const std::vector<std::function<void(Database&)>>& migrations)
{
    int userVersion = GetUserVersion(db);

    if (userVersion == migrations.size())
    {
        return true;
    }

    BOOST_LOG_TRIVIAL(info)
        << "Migrating database from version " << userVersion
        << " to version " << migrations.size();

    auto tx = db.BeginTransaction();

    for (int i = userVersion; i < migrations.size(); i++)
    {
        try
        {
            migrations[i](db);
        }
        catch (std::exception& ex)
        {
            BOOST_LOG_TRIVIAL(error) << "Failed to apply migration: " << ex.what();

            tx->Rollback();
            return false;
        }
    }

    SetUserVersion(db, static_cast<int>(migrations.size()));

    tx->Commit();

    return true;
}
