#pragma once
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    inline const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class VulkanPhysicalDevice {
      private:
        VkPhysicalDevice _device = VK_NULL_HANDLE;
        void pickPhysicalDevice(VkInstance instance);
        bool isDeviceSuitable(VkPhysicalDevice device) const;
        bool checkDeviceExtensionsSupport(VkPhysicalDevice device) const;
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

      public:
        VulkanPhysicalDevice(VkInstance instance);
        ~VulkanPhysicalDevice() = default;
        VulkanPhysicalDevice(const VulkanPhysicalDevice &) = delete;
        VulkanPhysicalDevice &operator=(const VulkanPhysicalDevice &) = delete;
        VkPhysicalDevice getPhysicalDevice() const {
            return _device;
        }
    };
} // namespace Engine::Renderer
