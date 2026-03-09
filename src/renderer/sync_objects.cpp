#include "sync_objects.hpp"
#include "core/logger.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    VulkanSyncObjects::VulkanSyncObjects(VkDevice device,
                                         uint32_t maxFramesInFlight)
        : _device(device) {
        _imageAvailableSemaphores.resize(maxFramesInFlight);
        _renderFinishedSemaphores.resize(maxFramesInFlight);
        _inFlightFences.resize(maxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < maxFramesInFlight; i++) {
            if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr,
                                  &_imageAvailableSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateSemaphore(_device, &semaphoreInfo, nullptr,
                                  &_renderFinishedSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateFence(_device, &fenceInfo, nullptr,
                              &_inFlightFences[i]) != VK_SUCCESS) {

                ENGINE_FATAL(
                    "Failed to create synchronization objects for a frame");
            }
        }
    }

    VulkanSyncObjects::~VulkanSyncObjects() {
        for (size_t i = 0; i < _imageAvailableSemaphores.size(); i++) {
            vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(_device, _inFlightFences[i], nullptr);
        }
    }
} // namespace Engine::Renderer
