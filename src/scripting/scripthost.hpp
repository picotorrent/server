#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace pt::Server::Scripting
{
    class ScriptHost
    {
    public:
        ScriptHost();
        ~ScriptHost();

        void Load(const fs::path& path);

    private:
        struct Runtime;
        struct Context;

        std::unique_ptr<Runtime> m_runtime;
        std::vector<std::unique_ptr<Context>> m_contexts;
    };
}
