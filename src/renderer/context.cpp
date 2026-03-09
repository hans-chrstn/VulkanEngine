#include "context.hpp"
#include "core/logger.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    VulkanContext::VulkanContext(GLFWwindow *window) {
        _instance = std::make_unique<VulkanInstance>();
        _surface =
            std::make_unique<VulkanSurface>(_instance->getInstance(), window);
        _gpu = std::make_unique<VulkanPhysicalDevice>(_instance->getInstance(),
                                                      _surface->getSurface());
        _device = std::make_unique<VulkanLogicalDevice>(*_gpu,
                                                        _surface->getSurface());
        _swapChain = std::make_unique<VulkanSwapChain>(
            *_gpu, *_device, _surface->getSurface(), window);
        _swapChainImageViews = std::make_unique<VulkanImageViews>(
            _device->getDevice(), _swapChain->getImages(),
            _swapChain->getFormat());
        _pipeline = std::make_unique<VulkanGraphicsPipeline>(
            _device->getDevice(), _swapChain->getExtent(),
            _swapChain->getFormat());
        _commandPool = std::make_unique<VulkanCommandPool>(
            _device->getDevice(),
            _gpu->getQueueFamilies().graphicsFamily.value());
        _commandBuffers = std::make_unique<VulkanCommandBuffers>(
            _device->getDevice(), _commandPool->getPool(),
            static_cast<uint32_t>(_swapChain->getImages().size()));
        _syncObjects = std::make_unique<VulkanSyncObjects>(
            _device->getDevice(), MAX_FRAMES_IN_FLIGHT);
    }
    VulkanContext::~VulkanContext() {
        vkDeviceWaitIdle(_device->getDevice());
    }

    void VulkanContext::drawFrame() {
        VkDevice device = _device->getDevice();

        vkWaitForFences(device, 1,
                        &_syncObjects->getInFlightFence()[_currentFrame],
                        VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(
            device, _swapChain->getSwapChain(), UINT64_MAX,
            _syncObjects->getImageAvailableSemaphores()[_currentFrame],
            VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            ENGINE_FATAL("Failed to acquire swap chain image!");
        }

        vkResetFences(device, 1,
                      &_syncObjects->getInFlightFence()[_currentFrame]);

        VkCommandBuffer commandBuffer =
            _commandBuffers->getCommandBuffers()[_currentFrame];
        vkResetCommandBuffer(commandBuffer, 0);
        recordCommandBuffer(commandBuffer, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {
            _syncObjects->getImageAvailableSemaphores()[_currentFrame]};
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore signalSemaphores[] = {
            _syncObjects->getRenderFinishedSemaphores()[_currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(_device->getGraphicsQueue(), 1, &submitInfo,
                          _syncObjects->getInFlightFence()[_currentFrame]) !=
            VK_SUCCESS) {
            ENGINE_FATAL("Failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {_swapChain->getSwapChain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(_device->getPresentQueue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        } else if (result != VK_SUCCESS) {
            ENGINE_FATAL("Failed to present swap chain image!");
        }

        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanContext::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                            uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            ENGINE_FATAL("Failed to begin recording command buffer!");
        }

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = _swapChain->getImages()[imageIndex];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
                             0, nullptr, 0, nullptr, 1, &barrier);

        VkRenderingAttachmentInfo colorAttachment{};
        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        colorAttachment.imageView =
            _swapChainImageViews->getImageViews()[imageIndex];
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue = {{0.01f, 0.01f, 0.01f, 1.0f}};

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea = {{0, 0}, _swapChain->getExtent()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;

        vkCmdBeginRendering(commandBuffer, &renderingInfo);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          _pipeline->getPipeline());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)_swapChain->getExtent().width;
        viewport.height = (float)_swapChain->getExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = _swapChain->getExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRendering(commandBuffer);

        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0,
                             nullptr, 0, nullptr, 1, &barrier);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            ENGINE_FATAL("Failed to record command buffer!");
        }
    }
} // namespace Engine::Renderer
