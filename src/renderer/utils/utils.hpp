#include <cstdint>
#include <utility>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace Engine::Renderer::Utils {
    template <typename T, typename Func, typename... Args>
    std::vector<T> fetchVulkanResources(Func enumerateFunc, Args &&...args) {
        uint32_t count = 0;
        enumerateFunc(std::forward<Args>(args)..., &count, nullptr);
        std::vector<T> resources(count);
        if (count > 0) {
            enumerateFunc(std::forward<Args>(args)..., &count,
                          resources.data());
        }
        return resources;
    }
} // namespace Engine::Renderer::Utils
