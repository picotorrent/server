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
            const std::string& name,
            const std::string& description,
            const libtorrent::settings_pack& settings);

        static std::shared_ptr<SettingsPack> GetById(sqlite3* db, int id);

        int Id() { return m_id; }
        std::string Name() { return m_name; }
        std::string Description() { return m_desc; }
        const libtorrent::settings_pack& Settings() { return m_settings; }

    private:
        SettingsPack(
            int id,
            const std::string& name,
            const std::string& description,
            libtorrent::settings_pack settings)
            : m_id(id),
            m_settings(std::move(settings))
        {
        }

        int m_id;
        std::string m_name;
        std::string m_desc;
        libtorrent::settings_pack m_settings;
    };
}
