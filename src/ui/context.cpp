#include "context.hpp"
#include "core/logger.hpp"
#include "renderer/buffer.hpp"
#include "renderer/pipeline.hpp"
#include "ui/vertex.hpp"
#include <array>
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Engine::UI {
    UIContext::UIContext(VkDevice device, VkPhysicalDevice physicalDevice,
                         VkFormat swapChainFormat, VkExtent2D extent)
        : _device(device), _extent(extent) {
        createPipeline(swapChainFormat, extent);
    }

    UIContext::~UIContext() {
        if (_descriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
        }
    }

    void UIContext::setFontTexture(VkImageView view, VkSampler sampler) {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize.descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = 1;

        if (vkCreateDescriptorPool(_device, &poolInfo, nullptr,
                                   &_descriptorPool) != VK_SUCCESS) {
            ENGINE_FATAL("Failed to create UI descriptor pool");
        }

        VkDescriptorSetLayout layouts[] = {
            _uiPipeline->getDescriptorSetLayout()};
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = layouts;

        if (vkAllocateDescriptorSets(_device, &allocInfo, &_descriptorSet) !=
            VK_SUCCESS) {
            ENGINE_FATAL("Failed to allocate UI descriptor set");
        }

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = view;
        imageInfo.sampler = sampler;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = _descriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);
    }

    void UIContext::addWidget(std::unique_ptr<Widget> widget) {
        _widgets.push_back(std::move(widget));
    }

    void UIContext::update(float deltaTime) {
        for (const auto &widget : _widgets) {
            widget->onUpdate(deltaTime);
        }
    }

    void UIContext::recordCommands(VkCommandBuffer commandBuffer) {
        if (_vertexBufferData.empty() || _descriptorSet == VK_NULL_HANDLE) {
            return;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          _uiPipeline->getPipeline());

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                _uiPipeline->getPipelineLayout(), 0, 1,
                                &_descriptorSet, 0, nullptr);

        glm::vec2 windowSize = {(float)_extent.width, (float)_extent.height};
        vkCmdPushConstants(commandBuffer, _uiPipeline->getPipelineLayout(),
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::vec2),
                           &windowSize);

        VkBuffer vertexBuffers[] = {_uiVertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, _uiIndexBuffer->getBuffer(), 0,
                             VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(_indexBufferData.size()), 1, 0,
                         0, 0);
    }

    void UIContext::createPipeline(VkFormat format, VkExtent2D extent) {
        auto binding = UIVertex::getBindingDescription();
        auto attributesArray = UIVertex::getAttributeDescription();
        std::vector<VkVertexInputAttributeDescription> attributes(
            attributesArray.begin(), attributesArray.end());

        VkDescriptorSetLayoutBinding samplerBinding{};
        samplerBinding.binding = 0;
        samplerBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerBinding.descriptorCount = 1;
        samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        samplerBinding.pImmutableSamplers = nullptr;

        std::vector<VkDescriptorSetLayoutBinding> bindings = {samplerBinding};

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(glm::vec2);

        _uiPipeline = std::make_unique<Renderer::VulkanGraphicsPipeline>(
            _device, extent, format, "shaders/ui_vert.spv",
            "shaders/ui_frag.spv", binding, attributes, bindings,
            &pushConstantRange, VK_CULL_MODE_NONE);
    }

    void UIContext::updateBuffers(VkPhysicalDevice physicalDevice) {
        _vertexBufferData.clear();
        _indexBufferData.clear();
        for (auto &widget : _widgets) {
            widget->onRender(_vertexBufferData, _indexBufferData);
        }

        if (_vertexBufferData.empty()) {
            return;
        }

        VkDeviceSize vSize = sizeof(UIVertex) * _vertexBufferData.size();
        VkDeviceSize iSize = sizeof(uint32_t) * _indexBufferData.size();

        if (!_uiVertexBuffer || _uiVertexBuffer->getSize() < vSize) {
            _uiVertexBuffer = std::make_unique<Renderer::VulkanBuffer>(
                _device, physicalDevice, vSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
        _uiVertexBuffer->copyTo(_vertexBufferData.data(), vSize);

        if (!_uiIndexBuffer || _uiIndexBuffer->getSize() < iSize) {
            _uiIndexBuffer = std::make_unique<Renderer::VulkanBuffer>(
                _device, physicalDevice, iSize,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
        _uiIndexBuffer->copyTo(_indexBufferData.data(), iSize);
    }
} // namespace Engine::UI
