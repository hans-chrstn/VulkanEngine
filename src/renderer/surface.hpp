#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
namespace Engine::Renderer {
    class VulkanSurface {
      private:
        VkInstance _instance;
        VkSurfaceKHR _surface;

      public:
        VulkanSurface(VkInstance instance, GLFWwindow *window);
        ~VulkanSurface();
        VulkanSurface(const VulkanSurface &) = delete;
        VulkanSurface &operator=(const VulkanSurface &) = delete;
        VkSurfaceKHR getSurface() const {
            return _surface;
        }
    };
} // namespace Engine::Renderer
