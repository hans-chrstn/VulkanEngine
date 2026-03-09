#pragma once
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

namespace Engine::Renderer {
    class VulkanContext {
      private:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t _currentFrame = 0;

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

        void recordCommandBuffer(VkCommandBuffer commandBuffer,
                                 uint32_t imageIndex);

      public:
        VulkanContext(GLFWwindow *window);
        ~VulkanContext();
        VulkanContext(const VulkanContext &) = delete;
        VulkanContext &operator=(const VulkanContext &) = delete;
        void drawFrame();
    };
} // namespace Engine::Renderer
