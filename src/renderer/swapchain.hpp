#pragma once
#include "logical_device.hpp"
#include "physical_device.hpp"
#include <GLFW/glfw3.h>

namespace Engine::Renderer {
    class VulkanSwapChain {
      private:
        VkDevice _device;
        VkSwapchainKHR _swapChain;
        std::vector<VkImage> _swapChainImages;
        VkFormat _swapChainImageFormat;
        VkExtent2D _swapChainExtent;

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D
        chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities,
                         GLFWwindow *window);

      public:
        VulkanSwapChain(const VulkanPhysicalDevice &physicalDevice,
                        const VulkanLogicalDevice &logicalDevice,
                        VkSurfaceKHR surface, GLFWwindow *window);
        ~VulkanSwapChain();
        VulkanSwapChain(const VulkanSwapChain &) = delete;
        VulkanSwapChain &operator=(const VulkanSwapChain &) = delete;
        VkSwapchainKHR getSwapChain() const {
            return _swapChain;
        }
        VkFormat getFormat() const {
            return _swapChainImageFormat;
        }
        VkExtent2D getExtent() const {
            return _swapChainExtent;
        }
        std::vector<VkImage> getImages() const {
            return _swapChainImages;
        }
    };
} // namespace Engine::Renderer
