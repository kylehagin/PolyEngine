#include "fs_util.h"
#include <fstream>

std::vector<char> ReadFileBinary(const std::filesystem::path& path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    f.seekg(0, std::ios::end);
    size_t size = (size_t)f.tellg();
    f.seekg(0);
    std::vector<char> data(size);
    f.read(data.data(), size);
    return data;
}

bool WriteFileBinary(const std::filesystem::path& path, const void* data, size_t bytes)
{
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    f.write((const char*)data, bytes);
    return true;
}

bool EnsureDir(const std::filesystem::path& p)
{
    std::error_code ec;
    if (std::filesystem::exists(p, ec)) return true;
    return std::filesystem::create_directories(p, ec);
}
