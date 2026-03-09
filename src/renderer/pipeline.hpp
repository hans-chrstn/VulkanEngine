#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    class VulkanGraphicsPipeline {
      private:
        VkDevice _device;
        VkPipelineLayout _pipelineLayout;
        VkPipeline _graphicsPipeline;
        VkDescriptorSetLayout _descriptorSetLayout;
        VkShaderModule createShaderModule(const std::vector<char> &code);

      public:
        VulkanGraphicsPipeline(VkDevice device, VkExtent2D extent,
                               VkFormat swapChainFormat);
        VulkanGraphicsPipeline(const VulkanGraphicsPipeline &) = delete;
        VulkanGraphicsPipeline &
        operator=(const VulkanGraphicsPipeline &) = delete;
        ~VulkanGraphicsPipeline();
        VkPipeline getPipeline() const {
            return _graphicsPipeline;
        }
        VkDescriptorSetLayout getDescriptorSetLayout() const {
            return _descriptorSetLayout;
        }
        VkPipelineLayout getPipelineLayout() const {
            return _pipelineLayout;
        }
    };
} // namespace Engine::Renderer
