#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include <libtorrent/settings_pack.hpp>
#include <sqlite3.h>

namespace pika::Data
{
    class SettingsPack
    {
    public:
        static std::shared_ptr<SettingsPack> Create(
            sqlite3* db,
            std::string_view const& name,
            std::string_view const& desc,
            libtorrent::settings_pack settings = libtorrent::default_settings());

        static std::shared_ptr<SettingsPack> GetById(sqlite3* db, int id);

        static std::unordered_set<std::string>& Names();

        int Id() { return m_id; }
        std::string& Name() { return m_name; }
        std::string& Description() { return m_desc; }
        libtorrent::settings_pack& Settings() { return m_settings; }

    private:
        SettingsPack(
            int id,
            std::string_view const& name,
            std::string_view const& desc,
            libtorrent::settings_pack const& settings)
            : m_id(id),
            m_name(name),
            m_desc(desc),
            m_settings(settings)
        {
        }

        int m_id;
        std::string m_name;
        std::string m_desc;
        libtorrent::settings_pack m_settings;
    };
}
