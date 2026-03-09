#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    class VulkanImageViews {
      private:
        VkDevice _device;
        std::vector<VkImageView> _imageViews;

      public:
        VulkanImageViews(VkDevice device, const std::vector<VkImage> &images,
                         VkFormat format);
        VulkanImageViews(const VulkanImageViews &) = delete;
        VulkanImageViews &operator=(const VulkanImageViews &) = delete;
        ~VulkanImageViews();
        std::vector<VkImageView> getImageViews() const;
    };
} // namespace Engine::Renderer
