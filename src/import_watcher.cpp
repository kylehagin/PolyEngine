#include "import_watcher.h"
#include "importer.h"
#include "log.h"
#include <filesystem>

ImportWatcher::ImportWatcher(const std::filesystem::path& r, Callback cb)
: root(r), callback(cb)
{
    lastScan = std::chrono::steady_clock::now();
}

void ImportWatcher::Tick()
{
    auto now = std::chrono::steady_clock::now();
    if (now - lastScan < std::chrono::seconds(2)) return;
    lastScan = now;

    for (auto& p : std::filesystem::recursive_directory_iterator(root))
    {
        if (!p.is_regular_file()) continue;
        auto ext = p.path().extension().string();
        if (ext != ".fbx" && ext != ".obj" && ext != ".gltf" && ext != ".glb")
            continue;
        auto mtime = std::filesystem::last_write_time(p);
        if (!known.count(p.path()) || known[p.path()] != mtime)
        {
            known[p.path()] = mtime;
            std::filesystem::path out = std::filesystem::path("Content/Packs") / p.path().stem();
            if (ImportAssetFile(p.path(), out))
            {
                if (callback) callback(out);
            }
        }
    }
}
