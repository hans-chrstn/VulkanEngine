#include "logical_device.hpp"
#include "core/logger.hpp"
#include "physical_device.hpp"
#include <cstdint>
#include <set>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    VulkanLogicalDevice::VulkanLogicalDevice(
        const VulkanPhysicalDevice &physicalDevice, VkSurfaceKHR surface) {
        QueueFamilyIndices indices = physicalDevice.findQueueFamilies(
            physicalDevice.getPhysicalDevice(), surface);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceVulkan13Features vulkanFeatures{};
        vulkanFeatures.sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vulkanFeatures.dynamicRendering = VK_TRUE;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = &vulkanFeatures;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount =
            static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount =
            static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.enabledLayerCount = 0;

        if (vkCreateDevice(physicalDevice.getPhysicalDevice(), &createInfo,
                           nullptr, &_device) != VK_SUCCESS) {
            ENGINE_FATAL("Failed to create logical device");
        }

        vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0,
                         &_graphicsQueue);
        vkGetDeviceQueue(_device, indices.presentFamily.value(), 0,
                         &_presentQueue);
    }

    VulkanLogicalDevice::~VulkanLogicalDevice() {
        vkDestroyDevice(_device, nullptr);
    }
} // namespace Engine::Renderer
