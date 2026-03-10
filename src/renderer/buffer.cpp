#include "buffer.hpp"
#include "core/logger.hpp"
#include "renderer/utils/utils.hpp"
#include <cstddef>
#include <cstring>

namespace Engine::Renderer {
    VulkanBuffer::VulkanBuffer(VkDevice device, VkPhysicalDevice physicalDevice,
                               VkDeviceSize size, VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties)
        : _device(device), _size(size) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(_device, &bufferInfo, nullptr, &_buffer) !=
            VK_SUCCESS) {
            ENGINE_FATAL("Failed to create buffer");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(_device, _buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Utils::findMemoryType(
            physicalDevice, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(_device, &allocInfo, nullptr, &_memory) !=
            VK_SUCCESS) {
            ENGINE_FATAL("Failed to allocate buffer memory");
        }

        vkBindBufferMemory(_device, _buffer, _memory, 0);
    }

    VulkanBuffer::~VulkanBuffer() {
        vkDestroyBuffer(_device, _buffer, nullptr);
        vkFreeMemory(_device, _memory, nullptr);
    }

    void VulkanBuffer::copyTo(void *data, VkDeviceSize size) {
        void *mappedData;

        vkMapMemory(_device, _memory, 0, size, 0, &mappedData);
        std::memcpy(mappedData, data, (size_t)size);
        vkUnmapMemory(_device, _memory);
    }
} // namespace Engine::Renderer
