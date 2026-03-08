#include "physical_device.hpp"
#include "core/logger.hpp"
#include <cstdint>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    QueueFamilyIndices
    VulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device) const {
        QueueFamilyIndices result;
        uint32_t count = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count,
                                                 queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                result.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            // NOTE: add vkgetphysicaldevicesurfacesupportkhr later
            if (presentSupport) {
                result.presentFamily = i;
            }

            if (result.isComplete()) {
                break;
            }
            i++;
        }

        return result;
    }

    bool VulkanPhysicalDevice::checkDeviceExtensionsSupport(
        VkPhysicalDevice device) const {
        uint32_t count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &count,
                                             availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                                 deviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool VulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device) const {
        QueueFamilyIndices familyIndices = findQueueFamilies(device);
        bool extensionsSupported = checkDeviceExtensionsSupport(device);
        // NOTE: add swapchain later
        return familyIndices.isComplete() && extensionsSupported;
    }

    void VulkanPhysicalDevice::pickPhysicalDevice(VkInstance instance) {
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);

        if (count == 0) {
            ENGINE_FATAL("Failed to find GPUs with Vulkan support");
        }

        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(instance, &count, devices.data());

        for (const auto &device : devices) {
            if (isDeviceSuitable(device)) {
                _device = device;
                break;
            }
        }

        if (_device == VK_NULL_HANDLE) {
            ENGINE_FATAL("Failed to find a suitable GPU");
        }
    }
} // namespace Engine::Renderer
