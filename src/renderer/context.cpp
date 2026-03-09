#include "context.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vulkan/vulkan_core.h>

namespace Engine::Renderer {
    VulkanContext::VulkanContext(GLFWwindow *window) {
        _instance = std::make_unique<VulkanInstance>();
        _surface =
            std::make_unique<VulkanSurface>(_instance->getInstance(), window);
        _gpu = std::make_unique<VulkanPhysicalDevice>(_instance->getInstance(),
                                                      _surface->getSurface());
        _device = std::make_unique<VulkanLogicalDevice>(*_gpu,
                                                        _surface->getSurface());
        _swapChain = std::make_unique<VulkanSwapChain>(
            *_gpu, *_device, _surface->getSurface(), window);
        _swapChainImageViews = std::make_unique<VulkanImageViews>(
            _device->getDevice(), _swapChain->getImages(),
            _swapChain->getFormat());
        _pipeline = std::make_unique<VulkanGraphicsPipeline>(
            _device->getDevice(), _swapChain->getExtent(),
            _swapChain->getFormat());
    }
    VulkanContext::~VulkanContext() {}
} // namespace Engine::Renderer
