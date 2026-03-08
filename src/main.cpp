#include "core/logger.hpp"
#include "core/window.hpp"
#include "renderer/context.hpp"
#include <GLFW/glfw3.h>
#include <exception>

int main() {
    Engine::Logger::Info("Vulkan Engine is initializing.");
    try {
        Engine::Core::Window window(1280, 720, "My Vulkan Engine");
        Engine::Renderer::VulkanContext context;
        while (!window.shouldClose()) {
            window.pollEvents();
            Engine::Logger::Info("Vulkan Engine has started!");
        }
    } catch (const std::exception &e) {
        ENGINE_FATAL(e.what());
        return 1;
    }
    return 0;
}
