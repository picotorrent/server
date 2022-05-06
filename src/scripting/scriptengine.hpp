#pragma once

#include <filesystem>

namespace pika::Scripting
{
    class IScriptEngine
    {
    public:
        virtual void LoadFile(const std::filesystem::path &path);
    };
}
