#include "context.hpp"
#include "core/logger.hpp"
#include "ubo.hpp"
#include "vertex.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <vector>
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
            _device->getDevice(),
            static_cast<uint32_t>(_swapChain->getImages().size()));

        _imagesInFlight.resize(_swapChain->getImages().size(), VK_NULL_HANDLE);

        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
            {{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
            {{0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

        const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

        VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
        _vertexBuffer = std::make_unique<VulkanBuffer>(
            _device->getDevice(), _gpu->getPhysicalDevice(), vertexBufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        _vertexBuffer->copyTo((void *)vertices.data(), vertexBufferSize);

        _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            _uniformBuffers[i] = std::make_unique<VulkanBuffer>(
                _device->getDevice(), _gpu->getPhysicalDevice(),
                sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }

        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(_device->getDevice(), &poolInfo, nullptr,
                                   &_descriptorPool) != VK_SUCCESS) {
            ENGINE_FATAL("Failed to create descriptor pool");
        }

        std::vector<VkDescriptorSetLayout> layouts(
            MAX_FRAMES_IN_FLIGHT, _pipeline->getDescriptorSetLayout());
        VkDescriptorSetAllocateInfo descriptorAllocInfo{};
        descriptorAllocInfo.sType =
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorAllocInfo.descriptorPool = _descriptorPool;
        descriptorAllocInfo.descriptorSetCount =
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        descriptorAllocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(_device->getDevice(), &descriptorAllocInfo,
                                     _descriptorSets.data()) != VK_SUCCESS) {
            ENGINE_FATAL("Failed to allocate descriptor sets");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = _uniformBuffers[i]->getBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = _descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(_device->getDevice(), 1, &descriptorWrite, 0,
                                   nullptr);
        }

        VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
        _indexBuffer = std::make_unique<VulkanBuffer>(
            _device->getDevice(), _gpu->getPhysicalDevice(), indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        _indexBuffer->copyTo((void *)indices.data(), indexBufferSize);
        _indexCount = static_cast<uint32_t>(indices.size());
    }
    VulkanContext::~VulkanContext() {
        vkDeviceWaitIdle(_device->getDevice());
    }

    void VulkanContext::updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                               glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f),
                                    _swapChain->getExtent().width /
                                        (float)_swapChain->getExtent().height,
                                    0.1f, 10.0f);
        ubo.proj[1][1] *= -1;
        _uniformBuffers[currentImage]->copyTo(&ubo, sizeof(ubo));
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

        if (_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device, 1, &_imagesInFlight[imageIndex], VK_TRUE,
                            UINT64_MAX);
        }

        _imagesInFlight[imageIndex] =
            _syncObjects->getInFlightFence()[_currentFrame];

        vkResetFences(device, 1,
                      &_syncObjects->getInFlightFence()[_currentFrame]);

        VkCommandBuffer commandBuffer =
            _commandBuffers->getCommandBuffers()[_currentFrame];
        vkResetCommandBuffer(commandBuffer, 0);
        updateUniformBuffer(_currentFrame);
        recordCommandBuffer(commandBuffer, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {
            _syncObjects->getImageAvailableSemaphores()[_currentFrame]};
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSemaphore signalSemaphores[] = {
            _syncObjects->getRenderFinishedSemaphores()[imageIndex]};

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
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

        VkBuffer vertexBuffers[] = {_vertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, _indexBuffer->getBuffer(), 0,
                             VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                _pipeline->getPipelineLayout(), 0, 1,
                                &_descriptorSets[_currentFrame], 0, nullptr);

        vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);

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
