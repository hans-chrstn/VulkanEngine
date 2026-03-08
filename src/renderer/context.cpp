#include "context.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    VulkanContext::VulkanContext(GLFWwindow *window) {
        _instance = std::make_unique<VulkanInstance>();
        _surface =
            std::make_unique<VulkanSurface>(_instance->getInstance(), window);
        _gpu = std::make_unique<VulkanPhysicalDevice>(_instance->getInstance(),
                                                      _surface->getSurface());
        _device = std::make_unique<VulkanLogicalDevice>(*_gpu,
                                                        _surface->getSurface());
    }
    VulkanContext::~VulkanContext() {}
} // namespace Engine::Renderer
