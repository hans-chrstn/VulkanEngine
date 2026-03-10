#pragma once
#include "renderer/buffer.hpp"
#include "renderer/pipeline.hpp"
#include "vertex.hpp"
#include "widget.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace Engine::UI {
    class UIContext {
      private:
        VkDevice _device;
        VkExtent2D _extent;

        std::vector<std::unique_ptr<Widget>> _widgets;
        std::unique_ptr<Renderer::VulkanGraphicsPipeline> _uiPipeline;
        std::unique_ptr<Renderer::VulkanBuffer> _uiVertexBuffer;
        std::unique_ptr<Renderer::VulkanBuffer> _uiIndexBuffer;

        std::vector<UIVertex> _vertexBufferData;
        std::vector<uint32_t> _indexBufferData;

        void createPipeline(VkFormat format, VkExtent2D extent);

      public:
        UIContext(VkDevice device, VkPhysicalDevice physicalDevice,
                  VkFormat swapChainFormat, VkExtent2D extent);
        ~UIContext();

        void addWidget(std::unique_ptr<Widget> widget);
        void update(float deltaTime);
        void updateBuffers(VkPhysicalDevice physicalDevice);
        void recordCommands(VkCommandBuffer commandBuffer);
    };
} // namespace Engine::UI
