#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace pt::Server::Data::Models
{
    class Profile
    {
    public:
        static std::shared_ptr<Profile> GetActive(sqlite3* db);
        static std::vector<std::shared_ptr<Profile>> GetAll(sqlite3* db);

        int Id() { return m_id; }
        std::string& Name() { return m_name; }
        bool IsActive() { return m_isActive; }
        std::string Description() { return m_desc; }
        std::optional<int> ProxyId() { return m_proxyId; }
        std::optional<std::string> ProxyName() { return m_proxyName; }
        int SettingsPackId() { return m_settingsPackId; }
        std::string& SettingsPackName() { return m_settingsPackName; }

    private:
        int m_id;
        std::string m_name;
        std::string m_desc;
        bool m_isActive;
        std::optional<int> m_proxyId;
        std::optional<std::string> m_proxyName;
        int m_settingsPackId;
        std::string m_settingsPackName;
    };
}
