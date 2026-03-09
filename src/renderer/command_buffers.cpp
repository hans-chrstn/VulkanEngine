#include "command_buffers.hpp"
#include "core/logger.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    VulkanCommandBuffers::VulkanCommandBuffers(VkDevice device,
                                               VkCommandPool pool,
                                               uint32_t count)
        : _device(device), _commandPool(pool) {
        _commandBuffers.resize(count);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = _commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = count;

        if (vkAllocateCommandBuffers(_device, &allocInfo,
                                     _commandBuffers.data()) != VK_SUCCESS) {
            ENGINE_FATAL("Failed to allocate command buffers");
        }
    }

    VulkanCommandBuffers::~VulkanCommandBuffers() {
        vkFreeCommandBuffers(_device, _commandPool,
                             static_cast<uint32_t>(_commandBuffers.size()),
                             _commandBuffers.data());
    }
} // namespace Engine::Renderer
