#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace Engine::Renderer {
    class VulkanTexture {
      private:
        VkDevice _device;
        VkImage _image = VK_NULL_HANDLE;
        VkDeviceMemory _memory = VK_NULL_HANDLE;
        VkImageView _view = VK_NULL_HANDLE;
        VkSampler _sampler = VK_NULL_HANDLE;

        void createTexture(VkPhysicalDevice physicalDevice,
                           VkCommandPool commandPool, VkQueue graphicsQueue,
                           const void *pixels, uint32_t width, uint32_t height,
                           VkFormat format, uint32_t bytesPerPixel);

      public:
        VulkanTexture(VkDevice device, VkPhysicalDevice physicalDevice,
                      VkCommandPool commandPool, VkQueue graphicsQueue,
                      const std::string &path);
        VulkanTexture(VkDevice device, VkPhysicalDevice physicalDevice,
                      VkCommandPool commandPool, VkQueue graphicsQueue,
                      const void *pixels, uint32_t width, uint32_t height,
                      VkFormat format = VK_FORMAT_R8_UNORM);
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
