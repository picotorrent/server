#pragma once

#include "../sessioneventhandler.hpp"

namespace pika::EventHandlers
{
    class SessionStatsHandler : public ISessionEventHandler
    {
    public:
        const std::map<std::string, int64_t>& Stats() { return m_stats; }

        void OnSessionStats(const std::map<std::string, int64_t> &stats) override
        {
            m_stats = stats;
        }

    private:
        std::map<std::string, int64_t> m_stats;
    };
}
