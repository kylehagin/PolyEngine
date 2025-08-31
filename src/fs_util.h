#pragma once
#include <string>
#include <vector>
#include <filesystem>

std::vector<char> ReadFileBinary(const std::filesystem::path& path);
bool WriteFileBinary(const std::filesystem::path& path, const void* data, size_t bytes);
bool EnsureDir(const std::filesystem::path& p);
