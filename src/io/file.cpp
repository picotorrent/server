#include "file.hpp"

#include <fstream>

using pt::Server::IO::File;

void File::ReadAllBytes(const fs::path &path, std::vector<char> &target)
{
    std::ifstream stream(path, std::ios::in | std::ios::binary);
    std::copy(
        std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>(),
        std::back_inserter(target));

}
