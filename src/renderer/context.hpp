#pragma once
#include "instance.hpp"
#include "logical_device.hpp"
#include "physical_device.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vulkan/vulkan.h>

namespace Engine::Renderer {
    class VulkanContext {
      private:
        std::unique_ptr<VulkanInstance> _instance;
        std::unique_ptr<VulkanSurface> _surface;
        std::unique_ptr<VulkanPhysicalDevice> _gpu;
        std::unique_ptr<VulkanLogicalDevice> _device;
        std::unique_ptr<VulkanSwapChain> _swapChain;

      public:
        VulkanContext(GLFWwindow *window);
        ~VulkanContext();
        VulkanContext(const VulkanContext &) = delete;
        VulkanContext &operator=(const VulkanContext &) = delete;
    };
} // namespace Engine::Renderer
