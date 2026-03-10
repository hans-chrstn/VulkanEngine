#include "physical_device.hpp"
#include "core/logger.hpp"
#include "utils/utils.hpp"
#include <set>

namespace Engine::Renderer {
    VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance,
                                               VkSurfaceKHR surface) {
        pickPhysicalDevice(instance, surface);
    }

    QueueFamilyIndices
    VulkanPhysicalDevice::findQueueFamilies(VkPhysicalDevice device,
                                            VkSurfaceKHR surface) const {
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
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
                                                 &presentSupport);
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
        std::vector<VkExtensionProperties> availableExtensions =
            Utils::fetchVulkanResources<VkExtensionProperties>(
                vkEnumerateDeviceExtensionProperties, device, nullptr);

        std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                                 deviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails
    VulkanPhysicalDevice::querySwapChainSupport(VkPhysicalDevice device,
                                                VkSurfaceKHR surface) const {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                                  &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                             nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                                 details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                                  &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device, surface, &presentModeCount,
                details.presentModes.data());
        }

        return details;
    }

    bool VulkanPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device,
                                                VkSurfaceKHR surface) const {
        QueueFamilyIndices familyIndices = findQueueFamilies(device, surface);
        bool extensionsSupported = checkDeviceExtensionsSupport(device);
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport =
                querySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.formats.empty() &&
                                !swapChainSupport.presentModes.empty();
        }
        return familyIndices.isComplete() && extensionsSupported &&
               swapChainAdequate;
    }

    void VulkanPhysicalDevice::pickPhysicalDevice(VkInstance instance,
                                                  VkSurfaceKHR surface) {
        std::vector<VkPhysicalDevice> devices =
            Utils::fetchVulkanResources<VkPhysicalDevice>(
                vkEnumeratePhysicalDevices, instance);

        if (devices.empty()) {
            ENGINE_FATAL("Failed to find GPUs with Vulkan support");
        }

        for (const auto &device : devices) {
            if (isDeviceSuitable(device, surface)) {
                _device = device;
                _indices = findQueueFamilies(device, surface);
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);
                ENGINE_INFO("Using GPU: " +
                            std::string(deviceProperties.deviceName));
                break;
            }
        }

        if (_device == VK_NULL_HANDLE) {
            ENGINE_FATAL("Failed to find a suitable GPU");
        }
    }
} // namespace Engine::Renderer
