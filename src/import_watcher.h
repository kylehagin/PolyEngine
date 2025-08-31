#pragma once
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <chrono>

class ImportWatcher
{
public:
    using Callback = std::function<void(const std::filesystem::path&)>;
    ImportWatcher(const std::filesystem::path& root, Callback cb);
    void Tick();
private:
    std::filesystem::path root;
    Callback callback;
    std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> known;
    std::chrono::steady_clock::time_point lastScan{};
};
