#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include <libtorrent/settings_pack.hpp>
#include <utility>
#include <sqlite3.h>

namespace pt::Server::Data
{
    class SettingsPack
    {
    public:
        static std::shared_ptr<SettingsPack> Create(
            sqlite3* db,
            int sessionId,
            const libtorrent::settings_pack& settings = libtorrent::default_settings());

        static std::shared_ptr<SettingsPack> GetById(sqlite3* db, int id);
        static std::shared_ptr<SettingsPack> GetBySessionId(sqlite3* db, int sessionId);

        int Id() { return m_id; }
        int SessionId() { return m_sessionId; }
        libtorrent::settings_pack& Settings() { return m_settings; }

    private:
        SettingsPack(
            int id,
            int sessionId,
            libtorrent::settings_pack settings)
            : m_id(id),
            m_sessionId(sessionId),
            m_settings(std::move(settings))
        {
        }

        static std::shared_ptr<SettingsPack> BuildFromStatement(sqlite3* db, sqlite3_stmt* stmt);

        int m_id;
        int m_sessionId;
        libtorrent::settings_pack m_settings;
    };
}
