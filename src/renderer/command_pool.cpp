#include "command_pool.hpp"
#include "core/logger.hpp"
#include <vulkan/vulkan.h>

namespace Engine::Renderer {
    VulkanCommandPool::VulkanCommandPool(VkDevice device,
                                         uint32_t queueFamilyIndex)
        : _device(device) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndex;

        if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) !=
            VK_SUCCESS) {
            ENGINE_FATAL("Failed to create command pool");
        }
    }

    VulkanCommandPool::~VulkanCommandPool() {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
    }
} // namespace Engine::Renderer
