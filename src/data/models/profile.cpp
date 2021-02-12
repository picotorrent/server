#include "profile.hpp"

#include <boost/log/trivial.hpp>

#include "../datareader.hpp"
#include "../sqliteexception.hpp"
#include "../statement.hpp"

using pt::Server::Data::Models::Profile;
using pt::Server::Data::SQLiteException;
using pt::Server::Data::Statement;

std::shared_ptr<Profile> Profile::GetActive(sqlite3* db)
{
    std::shared_ptr<Profile> result = nullptr;

    Statement::ForEach(
        db,
        "SELECT p.id, p.name, p.description, p.is_active, sp.id, sp.name, prx.id, prx.name "
        "FROM profiles p JOIN settings_pack sp ON p.settings_pack_id = sp.id "
        "LEFT JOIN proxy prx ON p.proxy_id = prx.id "
        "WHERE p.is_active = 1 "
        "LIMIT 1",
        [&](Statement::Row const& row)
        {
            result = Construct(row);
        });

    return result;
}

std::vector<std::shared_ptr<Profile>> Profile::GetAll(sqlite3* db)
{
    std::vector<std::shared_ptr<Profile>> result;

    Statement::ForEach(
        db,
        "SELECT p.id, p.name, p.description, p.is_active, sp.id, sp.name, prx.id, prx.name "
        "FROM profiles p JOIN settings_pack sp ON p.settings_pack_id = sp.id "
        "LEFT JOIN proxy prx ON p.proxy_id = prx.id "
        "ORDER BY p.id ASC",
        [&](Statement::Row const& row)
        {
            result.push_back(Construct(row));
        });

    return result;
}

std::shared_ptr<Profile> Profile::GetById(sqlite3* db, int id)
{
    std::shared_ptr<Profile> result;

    Statement::ForEach(
        db,
        "SELECT p.id, p.name, p.description, p.is_active, sp.id, sp.name, prx.id, prx.name "
        "FROM profiles p JOIN settings_pack sp ON p.settings_pack_id = sp.id "
        "LEFT JOIN proxy prx ON p.proxy_id = prx.id "
        "WHERE p.id = $1",
        [&](Statement::Row const& row)
        {
            result = Construct(row);
        },
        [&](sqlite3_stmt* stmt)
        {
            sqlite3_bind_int(stmt, 1, id);
        });

    return result;
}

void Profile::Update(sqlite3* db, std::shared_ptr<Profile> const& profile)
{
    Statement::ForEach(
        db,
        "UPDATE profiles SET proxy_id = $1 WHERE id = $2",
        [](auto const&) {},
        [&](sqlite3_stmt* stmt)
        {
            if (profile->ProxyId().has_value())
            {
                sqlite3_bind_int(stmt, 1, profile->ProxyId().value());
            }
            else
            {
                sqlite3_bind_null(stmt, 1);
            }

            sqlite3_bind_int(stmt, 2, profile->Id());
        });
}

std::shared_ptr<Profile> Profile::Construct(Statement::Row const& row)
{
    auto profile                = std::make_shared<Profile>();
    profile->m_id               = row.GetInt32(0);
    profile->m_name             = row.GetStdString(1);
    profile->m_desc             = row.GetStdString(2);
    profile->m_isActive         = row.GetBool(3);
    profile->m_settingsPackId   = row.GetInt32(4);
    profile->m_settingsPackName = row.GetStdString(5);

    if (!row.IsNull(6))
    {
        profile->m_proxyId = row.GetInt32(6);
    }

    if (!row.IsNull(7))
    {
        profile->m_proxyName = row.GetStdString(7);
    }

    return std::move(profile);
}
