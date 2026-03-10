#pragma once
#include <vulkan/vulkan.h>

namespace Engine::Renderer {
    class VulkanCommandPool {
      private:
        VkDevice _device;
        VkCommandPool _commandPool;

      public:
        VulkanCommandPool(VkDevice device, uint32_t queueFamilyIndex);
        VulkanCommandPool(const VulkanCommandPool &) = delete;
        VulkanCommandPool &operator=(const VulkanCommandPool &) = delete;
        ~VulkanCommandPool();
        VkCommandPool getPool() const {
            return _commandPool;
        }
    };
} // namespace Engine::Renderer
