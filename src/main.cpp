#include "core/logger.hpp"
#include "core/window.hpp"
#include "renderer/context.hpp"
#include <GLFW/glfw3.h>
#include <exception>

int main() {
    ENGINE_INFO("Vulkan Engine is initializing.");
    try {
        Engine::Core::Window window(1280, 720, "My Vulkan Engine");
        Engine::Renderer::VulkanContext context(window.getNativeWindow());
        while (!window.shouldClose()) {
            window.pollEvents();
            ENGINE_INFO("Vulkan Engine has started!");
        }
    } catch (const std::exception &e) {
        ENGINE_FATAL(e.what());
        return 1;
    }
    return 0;
}
