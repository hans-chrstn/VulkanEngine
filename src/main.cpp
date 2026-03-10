#include "core/input.hpp"
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
        Engine::Core::Input::setWindow(window.getNativeWindow());
        while (!window.shouldClose()) {
            window.pollEvents();
            context.drawFrame(window.getNativeWindow());
        }
    } catch (const std::exception &e) {
        ENGINE_FATAL(e.what());
        return 1;
    }
    return 0;
}
