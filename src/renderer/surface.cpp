#define GLFW_INCLUDE_VULKAN
#include "surface.hpp"
#include "core/logger.hpp"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
namespace Engine::Renderer {
    VulkanSurface::VulkanSurface(VkInstance instance, GLFWwindow *window)
        : _instance(instance) {
        if (glfwCreateWindowSurface(_instance, window, nullptr, &_surface) !=
            VK_SUCCESS) {
            ENGINE_FATAL("Failed to create window surface");
        }
    }

    VulkanSurface::~VulkanSurface() {
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
    }

} // namespace Engine::Renderer
