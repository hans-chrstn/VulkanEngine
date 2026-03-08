#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace Engine::Renderer {
    inline const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
#ifdef NDEBUG
    inline const bool enableValidationLayers = false;
#else
    inline const bool enableValidationLayers = true;
#endif
    class VulkanInstance {
      private:
        VkInstance _instance;
        bool checkValidationLayerSupport();

      public:
        VulkanInstance();
        ~VulkanInstance();
        VulkanInstance(const VulkanInstance &) = delete;
        VulkanInstance &operator=(const VulkanInstance &) = delete;
        VkInstance getInstance() const {
            return _instance;
        }
        std::vector<const char *> getRequiredExtensions();
    };
} // namespace Engine::Renderer
