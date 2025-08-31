#pragma once
#include "renderer.h"
#include "import_watcher.h"
#include <memory>

class App
{
public:
    bool Init(bool enableValidation);
    void Run();
    void Shutdown();
private:
    VkContext ctx{};
    Renderer renderer;
    std::unique_ptr<ImportWatcher> watcher;
};
