#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace Engine::Renderer {
    class VulkanSyncObjects {
      private:
        VkDevice _device;
        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;

      public:
        VulkanSyncObjects(VkDevice device, uint32_t maxFramesInFlight);
        VulkanSyncObjects(const VulkanSyncObjects &) = delete;
        VulkanSyncObjects &operator=(const VulkanSyncObjects &) = delete;
        ~VulkanSyncObjects();
        const std::vector<VkSemaphore>& getImageAvailableSemaphores() const {
            return _imageAvailableSemaphores;
        }
        const std::vector<VkSemaphore>& getRenderFinishedSemaphores() const {
            return _renderFinishedSemaphores;
        }
        const std::vector<VkFence>& getInFlightFence() const {
            return _inFlightFences;
        }
    };
} // namespace Engine::Renderer
