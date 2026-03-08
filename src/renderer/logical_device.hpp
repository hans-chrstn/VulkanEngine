#pragma once
#include "physical_device.hpp"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    class VulkanLogicalDevice {
      private:
        VkDevice _device;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;

      public:
        VulkanLogicalDevice(const VulkanPhysicalDevice &physicalDevice,
                            VkSurfaceKHR surface);
        VulkanLogicalDevice(const VulkanLogicalDevice &) = delete;
        VulkanLogicalDevice &operator=(const VulkanLogicalDevice &) = delete;
        ~VulkanLogicalDevice();

        VkDevice getDevice() const {
            return _device;
        }
        VkQueue getGraphicsQueue() const {
            return _graphicsQueue;
        }
        VkQueue getPresentQueue() const {
            return _presentQueue;
        }
    };
} // namespace Engine::Renderer
