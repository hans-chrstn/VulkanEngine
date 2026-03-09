#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    class VulkanBuffer {
      private:
        VkDevice _device;
        VkBuffer _buffer = VK_NULL_HANDLE;
        VkDeviceMemory _memory = VK_NULL_HANDLE;
        VkDeviceSize _size;

      public:
        VulkanBuffer(VkDevice device, VkPhysicalDevice VkPhysicalDevice,
                     VkDeviceSize size, VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties);
        ~VulkanBuffer();

        VulkanBuffer(const VulkanBuffer &) = delete;
        VulkanBuffer &operator=(const VulkanBuffer &) = delete;

        VkBuffer getBuffer() const {
            return _buffer;
        }
        void copyTo(void *data, VkDeviceSize size);
    };
} // namespace Engine::Renderer
