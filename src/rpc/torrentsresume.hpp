#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "command.hpp"

namespace pt::Server { class ITorrentHandleFinder; }

namespace pt::Server::RPC
{
    class TorrentsResumeCommand : public Command
    {
    public:
        TorrentsResumeCommand(std::shared_ptr<ITorrentHandleFinder>);
        nlohmann::json Execute(const nlohmann::json&) override;

    private:
        std::shared_ptr<ITorrentHandleFinder> m_finder;
    };
}
