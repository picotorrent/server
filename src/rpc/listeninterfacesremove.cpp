#include "listeninterfacesremove.hpp"

#include <boost/log/trivial.hpp>

#include "../data/datareader.hpp"
#include "../sessionmanager.hpp"

using json = nlohmann::json;
using pt::Server::RPC::ListenInterfacesRemoveCommand;
using pt::Server::SessionManager;

ListenInterfacesRemoveCommand::ListenInterfacesRemoveCommand(sqlite3* db, std::shared_ptr<SessionManager> session)
    : m_db(db),
    m_session(session)
{
}

json ListenInterfacesRemoveCommand::Execute(json& params)
{
    if (params.is_array())
    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(
            m_db,
            "DELETE FROM listen_interfaces WHERE id = $1",
            -1,
            &stmt,
            nullptr);

        for (auto const& p : params)
        {
            int id = p.get<int>();
            sqlite3_bind_int(stmt, 1, id);
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
        }

        sqlite3_finalize(stmt);

        // Reload session settings to reload listen interfaces
        m_session->ReloadSettings();
    }

    return true;
}
