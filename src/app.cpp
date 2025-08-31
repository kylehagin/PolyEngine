#include "app.h"
#include <GLFW/glfw3.h>
#include "log.h"

GLFWwindow* g_Window = nullptr;

bool App::Init(bool enableValidation)
{
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    g_Window = glfwCreateWindow(1280, 720, "synty-lite", nullptr, nullptr);

    if (!InitVulkan(ctx, g_Window, enableValidation))
        return false;

    renderer.Init(ctx, 1280, 720);

    watcher = std::make_unique<ImportWatcher>("Content/Drop",
        [this](const std::filesystem::path& out){ Log("Imported %s", out.string().c_str()); });

    return true;
}

void App::Run()
{
    while(!glfwWindowShouldClose(g_Window))
    {
        glfwPollEvents();
        watcher->Tick();
        renderer.DrawFrame();
    }
}

void App::Shutdown()
{
    renderer.Shutdown();
    ShutdownVulkan(ctx);
    glfwDestroyWindow(g_Window);
    glfwTerminate();
}
