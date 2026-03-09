#pragma once
#include "core/logger.hpp"
#include <cstdint>
#include <fstream>
#include <string>
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

    inline std::vector<char> readFile(const std::string &fileName) {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            ENGINE_FATAL("Failed to open file: " + fileName);
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }
} // namespace Engine::Renderer::Utils
