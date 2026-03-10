#include "context.hpp"
#include "renderer/buffer.hpp"
#include "renderer/pipeline.hpp"
#include "ui/vertex.hpp"
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

    UIContext::~UIContext() {}

    void UIContext::addWidget(std::unique_ptr<Widget> widget) {
        _widgets.push_back(std::move(widget));
    }

    void UIContext::update(float deltaTime) {
        _vertexBufferData.clear();
        _indexBufferData.clear();

        for (const auto &widget : _widgets) {
            widget->onUpdate(deltaTime);
        }
    }

    void UIContext::recordCommands(VkCommandBuffer commandBuffer) {
        if (_vertexBufferData.empty()) {
            return;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          _uiPipeline->getPipeline());

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

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(glm::vec2);

        _uiPipeline = std::make_unique<Renderer::VulkanGraphicsPipeline>(
            _device, extent, format, "shaders/ui_vert.spv",
            "shaders/ui_frag.spv", binding, attributes, &pushConstantRange,
            VK_CULL_MODE_NONE);
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

        VkDeviceSize vertexBufferSize =
            sizeof(UIVertex) * _vertexBufferData.size();
        VkDeviceSize indexBufferSize =
            sizeof(uint32_t) * _indexBufferData.size();

        if (!_uiVertexBuffer || _uiVertexBuffer->getSize() < vertexBufferSize) {
            _uiVertexBuffer = std::make_unique<Renderer::VulkanBuffer>(
                _device, physicalDevice, vertexBufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
        _uiVertexBuffer->copyTo(_vertexBufferData.data(), vertexBufferSize);

        if (!_uiIndexBuffer || _uiIndexBuffer->getSize() < indexBufferSize) {
            _uiIndexBuffer = std::make_unique<Renderer::VulkanBuffer>(
                _device, physicalDevice, indexBufferSize,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
        _uiIndexBuffer->copyTo(_indexBufferData.data(), indexBufferSize);
    }
} // namespace Engine::UI
