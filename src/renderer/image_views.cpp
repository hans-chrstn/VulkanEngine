#include "image_views.hpp"
#include "core/logger.hpp"
#include <vulkan/vulkan.h>

namespace Engine::Renderer {
    VulkanImageViews::VulkanImageViews(VkDevice device,
                                       const std::vector<VkImage> &images,
                                       VkFormat format)
        : _device(device) {
        _imageViews.resize(images.size());
        for (size_t i = 0; i < images.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr,
                                  &_imageViews[i]) != VK_SUCCESS) {
                ENGINE_FATAL("Failed to create image views");
            }
        }
    }

    VulkanImageViews::~VulkanImageViews() {
        for (auto imageView : _imageViews) {
            vkDestroyImageView(_device, imageView, nullptr);
        }
    }

    std::vector<VkImageView> VulkanImageViews::getImageViews() const {
        return _imageViews;
    }
} // namespace Engine::Renderer
