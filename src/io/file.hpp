#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace pt::Server::IO
{
    class File
    {
    public:
        static void ReadAllBytes(const fs::path& path, std::vector<char>& target);
    };
}
