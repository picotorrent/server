#include "sessionmanager.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <utility>
#include <libtorrent/write_resume_data.hpp>
#include <nlohmann/json.hpp>

#include "data/datareader.hpp"
#include "data/sqliteexception.hpp"
#include "data/models/settingspack.hpp"
#include "data/statement.hpp"
#include "json/infohash.hpp"
#include "json/torrentstatus.hpp"
#include "session.hpp"
#include "tsdb/influxdb.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::SettingsPack;
using pt::Server::Data::Statement;
using pt::Server::Data::SQLiteException;
using pt::Server::ISession;
using pt::Server::ISessionManager;
using pt::Server::Session;
using pt::Server::SessionManager;

std::shared_ptr<ISessionManager> SessionManager::Load(boost::asio::io_context& io, sqlite3* db)
{
    std::vector<std::shared_ptr<ISession>> sessions;

    Statement::ForEach(
        db,
        "SELECT id,name,params_data,settings_pack_id FROM sessions",
        [&](const Statement::Row& row)
        {
            int id = row.GetInt32(0);
            std::string name = row.GetStdString(1);

            BOOST_LOG_TRIVIAL(info) << "Loading session " << name;

            lt::session_params params;

            if (!row.IsNull(2))
            {
                auto buffer = row.GetBlob(2);

                lt::error_code ec;
                lt::bdecode_node node = lt::bdecode(
                    lt::span<const char>(buffer.data(), buffer.size()),
                    ec);

                if (ec)
                {
                    BOOST_LOG_TRIVIAL(warning) << "Failed to bdecode session params buffer: " << ec.message();
                }
                else
                {
                    params = lt::read_session_params(node, lt::session::save_dht_state);
                    BOOST_LOG_TRIVIAL(info) << "Loaded session params (" << buffer.size() << " bytes)";
                }
            }

            auto settingsPack = SettingsPack::GetById(db, row.GetInt32(3));
            params.settings = settingsPack->Settings();

            sessions.push_back(std::make_shared<Session>(io, db, id, name, params));
        });

    auto sm = std::shared_ptr<SessionManager>(
        new SessionManager(sessions));

    return std::move(sm);
}

SessionManager::SessionManager(std::vector<std::shared_ptr<ISession>>  sessions)
    : m_sessions(std::move(sessions))
{
}

SessionManager::~SessionManager()
{
}

std::weak_ptr<ISession> SessionManager::Get(int sessionId)
{
    throw std::exception();
}

std::weak_ptr<ISession> SessionManager::GetDefault()
{
    throw std::exception();
}
