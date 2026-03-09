#pragma once
#include "buffer.hpp"
#include "command_buffers.hpp"
#include "command_pool.hpp"
#include "image_views.hpp"
#include "instance.hpp"
#include "logical_device.hpp"
#include "physical_device.hpp"
#include "pipeline.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "sync_objects.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <memory>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    class VulkanContext {
      private:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 3;
        uint32_t _currentFrame = 0;
        uint32_t _vertexCount = 0;
        uint32_t _indexCount = 0;

        VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;

        std::unique_ptr<VulkanInstance> _instance;
        std::unique_ptr<VulkanSurface> _surface;
        std::unique_ptr<VulkanPhysicalDevice> _gpu;
        std::unique_ptr<VulkanLogicalDevice> _device;
        std::unique_ptr<VulkanSwapChain> _swapChain;
        std::unique_ptr<VulkanImageViews> _swapChainImageViews;
        std::unique_ptr<VulkanGraphicsPipeline> _pipeline;
        std::unique_ptr<VulkanCommandPool> _commandPool;
        std::unique_ptr<VulkanCommandBuffers> _commandBuffers;
        std::unique_ptr<VulkanSyncObjects> _syncObjects;
        std::unique_ptr<VulkanBuffer> _vertexBuffer;
        std::unique_ptr<VulkanBuffer> _indexBuffer;

        std::vector<std::unique_ptr<VulkanBuffer>> _uniformBuffers;
        std::vector<VkDescriptorSet> _descriptorSets;
        std::vector<VkFence> _imagesInFlight;

        void recordCommandBuffer(VkCommandBuffer commandBuffer,
                                 uint32_t imageIndex);

        void updateUniformBuffer(uint32_t currentImage);

      public:
        VulkanContext(GLFWwindow *window);
        ~VulkanContext();
        VulkanContext(const VulkanContext &) = delete;
        VulkanContext &operator=(const VulkanContext &) = delete;
        void drawFrame();
    };
} // namespace Engine::Renderer
