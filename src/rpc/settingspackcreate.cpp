#include "settingspackcreate.hpp"

#include <boost/log/trivial.hpp>
#include <libtorrent/settings_pack.hpp>

#include "../data/datareader.hpp"
#include "../data/models/settingspack.hpp"
#include "../sessionmanager.hpp"

namespace lt = libtorrent;
using json = nlohmann::json;
using pt::Server::Data::SettingsPack;
using pt::Server::RPC::SettingsPackCreateCommand;
using pt::Server::SessionManager;

SettingsPackCreateCommand::SettingsPackCreateCommand(sqlite3* db)
    : m_db(db)
{
}

json SettingsPackCreateCommand::Execute(json& params)
{
    return Error(-1, "not implemented");
}
