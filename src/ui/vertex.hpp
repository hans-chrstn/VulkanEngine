#pragma once
#include <array>
#include <cstddef>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace Engine::UI {
    struct UIVertex {
        glm::vec2 pos;
        glm::vec4 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(UIVertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3>
        getAttributeDescription() {
            std::array<VkVertexInputAttributeDescription, 3> attributes{};
            attributes[0] = {0, 0, VK_FORMAT_R32G32_SFLOAT,
                             offsetof(UIVertex, pos)};
            attributes[1] = {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT,
                             offsetof(UIVertex, color)};
            attributes[2] = {2, 0, VK_FORMAT_R32G32_SFLOAT,
                             offsetof(UIVertex, texCoord)};
            return attributes;
        }
    };
} // namespace Engine::UI
