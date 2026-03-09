#pragma once

#include <string>
#include <vulkan/vulkan_core.h>
namespace Engine::Renderer {
    class VulkanTexture {
      private:
        VkDevice _device;
        VkImage _image = VK_NULL_HANDLE;
        VkDeviceMemory _memory = VK_NULL_HANDLE;
        VkImageView _view = VK_NULL_HANDLE;
        VkSampler _sampler = VK_NULL_HANDLE;

      public:
        VulkanTexture(VkDevice device, VkPhysicalDevice physicalDevice,
                      VkCommandPool commandPool, VkQueue graphicsQueue,
                      const std::string &path);
        ~VulkanTexture();

        VulkanTexture(const VulkanTexture &) = delete;
        VulkanTexture &operator=(const VulkanTexture &) = delete;

        VkImageView getView() const {
            return _view;
        }
        VkSampler getSampler() const {
            return _sampler;
        }
    };
} // namespace Engine::Renderer
