#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace Engine::Renderer {
    class VulkanCommandBuffers {
      private:
        VkDevice _device;
        VkCommandPool _commandPool;
        std::vector<VkCommandBuffer> _commandBuffers;

      public:
        VulkanCommandBuffers(VkDevice device, VkCommandPool pool,
                             uint32_t count);
        VulkanCommandBuffers(const VulkanCommandBuffers &) = delete;
        VulkanCommandBuffers &operator=(const VulkanCommandBuffers &) = delete;
        ~VulkanCommandBuffers();
        std::vector<VkCommandBuffer> getCommandBuffers() const {
            return _commandBuffers;
        }
    };
} // namespace Engine::Renderer
