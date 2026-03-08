#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    inline const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
#ifdef NDEBUG
    inline const bool enableValidationLayers = false;
#else
    inline const bool enableValidationLayers = true;
#endif
    class VulkanContext {
      private:
        VkInstance _instance;

        bool checkValidationLayerSupport();

      public:
        VulkanContext();
        ~VulkanContext();
        VulkanContext(const VulkanContext &) = delete;
        VulkanContext &operator=(const VulkanContext &) = delete;
        VkInstance getInstance() const {
            return _instance;
        }
        std::vector<const char *> getRequiredExtensions();
    };
} // namespace Engine::Renderer
