#include "context.hpp"
#include "core/input.hpp"
#include "core/logger.hpp"
#include "model_loader.hpp"
#include "ubo.hpp"
#include "ui/context.hpp"
#include "ui/layout_manager.hpp"
#include "ui/panel.hpp"
#include "ui/text.hpp"
#include "vertex.hpp"
#include <chrono>
#include <memory>

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

        _camera = std::make_unique<Camera>();

        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::vector<VkDescriptorSetLayoutBinding> bindings = {
            uboLayoutBinding, samplerLayoutBinding};

        auto attr = Vertex::getAttributeDescriptions();
        std::vector<VkVertexInputAttributeDescription> attributeVector(
            attr.begin(), attr.end());

        _pipeline = std::make_unique<VulkanGraphicsPipeline>(
            _device->getDevice(), _swapChain->getExtent(),
            _swapChain->getFormat(), "shaders/vert.spv", "shaders/frag.spv",
            Vertex::getBindingDescription(), attributeVector, bindings, nullptr,
            VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);

        _commandPool = std::make_unique<VulkanCommandPool>(
            _device->getDevice(),
            _gpu->getQueueFamilies().graphicsFamily.value());
        _commandBuffers = std::make_unique<VulkanCommandBuffers>(
            _device->getDevice(), _commandPool->getPool(),
            static_cast<uint32_t>(_swapChain->getImages().size()));
        _syncObjects = std::make_unique<VulkanSyncObjects>(
            _device->getDevice(),
            static_cast<uint32_t>(_swapChain->getImages().size()));
        _texture = std::make_unique<VulkanTexture>(
            _device->getDevice(), _gpu->getPhysicalDevice(),
            _commandPool->getPool(), _device->getGraphicsQueue(),
            "textures/texture.png");

        _font = std::make_unique<Font>(
            _device->getDevice(), _gpu->getPhysicalDevice(),
            _commandPool->getPool(), _device->getGraphicsQueue(),
            "assets/fonts/JetBrains-Mono/ttf/JetBrainsMono-Regular.ttf");

        _imagesInFlight.resize(_swapChain->getImages().size(), VK_NULL_HANDLE);

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        loadModel("models/model.obj", vertices, indices);
        _indexCount = static_cast<uint32_t>(indices.size());

        VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
        _vertexBuffer = std::make_unique<VulkanBuffer>(
            _device->getDevice(), _gpu->getPhysicalDevice(), vertexBufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        _vertexBuffer->copyTo((void *)vertices.data(), vertexBufferSize);

        VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
        _indexBuffer = std::make_unique<VulkanBuffer>(
            _device->getDevice(), _gpu->getPhysicalDevice(), indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        _indexBuffer->copyTo((void *)indices.data(), indexBufferSize);

        _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            _uniformBuffers[i] = std::make_unique<VulkanBuffer>(
                _device->getDevice(), _gpu->getPhysicalDevice(),
                sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }

        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount =
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount =
            static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
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

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _texture->getView();
            imageInfo.sampler = _texture->getSampler();

            VkWriteDescriptorSet uboDescriptorWrite{};
            uboDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            uboDescriptorWrite.dstSet = _descriptorSets[i];
            uboDescriptorWrite.dstBinding = 0;
            uboDescriptorWrite.dstArrayElement = 0;
            uboDescriptorWrite.descriptorType =
                VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboDescriptorWrite.descriptorCount = 1;
            uboDescriptorWrite.pBufferInfo = &bufferInfo;

            VkWriteDescriptorSet samplerDescriptorWrite{};
            samplerDescriptorWrite.sType =
                VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            samplerDescriptorWrite.dstSet = _descriptorSets[i];
            samplerDescriptorWrite.dstBinding = 1;
            samplerDescriptorWrite.dstArrayElement = 0;
            samplerDescriptorWrite.descriptorType =
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerDescriptorWrite.descriptorCount = 1;
            samplerDescriptorWrite.pImageInfo = &imageInfo;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites = {
                uboDescriptorWrite, samplerDescriptorWrite};
            vkUpdateDescriptorSets(
                _device->getDevice(),
                static_cast<uint32_t>(descriptorWrites.size()),
                descriptorWrites.data(), 0, nullptr);
        }

        _uiContext = std::make_unique<UI::UIContext>(
            _device->getDevice(), _gpu->getPhysicalDevice(),
            _swapChain->getFormat(), _swapChain->getExtent());
        _uiContext->setFontTexture(_font->getAtlas().getView(),
                                   _font->getAtlas().getSampler());

        uint32_t w = _swapChain->getExtent().width;
        uint32_t h = _swapChain->getExtent().height;

        auto hRect = UI::LayoutManager::GetPanelRect(UI::PanelSide::Left, w, h);
        auto pRect =
            UI::LayoutManager::GetPanelRect(UI::PanelSide::Bottom, w, h);
        auto iRect =
            UI::LayoutManager::GetPanelRect(UI::PanelSide::Right, w, h);
        auto tRect = UI::LayoutManager::GetPanelRect(UI::PanelSide::Top, w, h);

        _uiContext->addWidget(std::make_unique<UI::UIPanel>(
            hRect, glm::vec4(0.15f, 0.15f, 0.15f, 0.5f)));
        _uiContext->addWidget(std::make_unique<UI::UIPanel>(
            pRect, glm::vec4(0.12f, 0.12f, 0.12f, 0.5f)));
        _uiContext->addWidget(std::make_unique<UI::UIPanel>(
            iRect, glm::vec4(0.15f, 0.15f, 0.15f, 0.5f)));
        _uiContext->addWidget(std::make_unique<UI::UIPanel>(
            tRect, glm::vec4(0.18f, 0.18f, 0.18f, 0.5f)));

        UI::Rect labelRect = {hRect.x + 5.0f, hRect.y + 5.0f, 100.0f, 20.0f};
        _uiContext->addWidget(std::make_unique<UI::UIText>(
            "Hierarchy", labelRect, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.3f,
            *_font));
    }

    VulkanContext::~VulkanContext() {
        vkDeviceWaitIdle(_device->getDevice());
        vkDestroyDescriptorPool(_device->getDevice(), _descriptorPool, nullptr);
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
        ubo.view = _camera->GetViewMatrix();
        float aspect = _swapChain->getExtent().width /
                       (float)_swapChain->getExtent().height;
        ubo.proj = glm::perspective(glm::radians(_camera->zoom), aspect, 0.1f,
                                    1000.0f);
        ubo.proj[1][1] *= -1;
        _uniformBuffers[currentImage]->copyTo(&ubo, sizeof(ubo));
    }

    void VulkanContext::drawFrame(GLFWwindow *window) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - _lastFrameTime;
        _lastFrameTime = currentFrameTime;

        _camera->zoom_input(Core::Input::scroll_offset);
        Core::Input::scroll_offset = 0.0f;

        if (Core::Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            double xpos = Core::Input::getMouseX();
            double ypos = Core::Input::getMouseY();
            if (_firstMouse) {
                _lastMouseX = xpos;
                _lastMouseY = ypos;
                _firstMouse = false;
            }
            float xoffset = static_cast<float>(xpos - _lastMouseX);
            float yoffset = static_cast<float>(_lastMouseY - ypos);
            _lastMouseX = xpos;
            _lastMouseY = ypos;
            _camera->rotate(xoffset, yoffset);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            _firstMouse = true;
        }

        _camera->update(deltaTime);

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

        vkResetFences(device, 1,
                      &_syncObjects->getInFlightFence()[_currentFrame]);
        _imagesInFlight[imageIndex] =
            _syncObjects->getInFlightFence()[_currentFrame];

        VkCommandBuffer commandBuffer =
            _commandBuffers->getCommandBuffers()[_currentFrame];
        vkResetCommandBuffer(commandBuffer, 0);
        updateUniformBuffer(_currentFrame);

        _uiContext->update(deltaTime);
        _uiContext->updateBuffers(_gpu->getPhysicalDevice());

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
                             VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                _pipeline->getPipelineLayout(), 0, 1,
                                &_descriptorSets[_currentFrame], 0, nullptr);
        vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);

        _uiContext->recordCommands(commandBuffer);

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
