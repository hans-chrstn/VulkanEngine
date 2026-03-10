#pragma once
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace Engine::Renderer {
    inline const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

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
        QueueFamilyIndices _indices;

        void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
        bool isDeviceSuitable(VkPhysicalDevice device,
                              VkSurfaceKHR surface) const;
        bool checkDeviceExtensionsSupport(VkPhysicalDevice device) const;

      public:
        VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
        ~VulkanPhysicalDevice() = default;
        VulkanPhysicalDevice(const VulkanPhysicalDevice &) = delete;
        VulkanPhysicalDevice &operator=(const VulkanPhysicalDevice &) = delete;
        VkPhysicalDevice getPhysicalDevice() const {
            return _device;
        }
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                             VkSurfaceKHR surface) const;
        SwapChainSupportDetails
        querySwapChainSupport(VkPhysicalDevice device,
                              VkSurfaceKHR surface) const;

        QueueFamilyIndices getQueueFamilies() const {
            return _indices;
        };
    };
} // namespace Engine::Renderer
