#pragma once
#include "core/input.hpp"
#include "vertex.hpp"
#include "widget.hpp"
#include <cstdint>
#include <glm/fwd.hpp>
#include <vector>

namespace Engine::UI {
    class UIPanel : public Widget {
      private:
        glm::vec4 _baseColor;
        glm::vec4 _hoverColor;

      public:
        UIPanel(Rect bounds, glm::vec4 color) {
            _bounds = bounds;
            _baseColor = color;
            _hoverColor = color + glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
        }

        void onUpdate(float deltaTime) override {
            auto mousePos = Core::Input::getMousePosition();
            _isHovered = _bounds.contains(mousePos.x, mousePos.y);
        }

        void onRender(std::vector<UIVertex> &outVertices,
                      std::vector<uint32_t> &outIndices) override {
            glm::vec4 color = _isHovered ? _hoverColor : _baseColor;
            uint32_t offset = static_cast<uint32_t>(outVertices.size());

            outVertices.push_back(
                {{_bounds.x, _bounds.y}, color, {-1.0f, -1.0f}});
            outVertices.push_back({{_bounds.x + _bounds.width, _bounds.y},
                                   color,
                                   {-1.0f, -1.0f}});
            outVertices.push_back(
                {{_bounds.x + _bounds.width, _bounds.y + _bounds.height},
                 color,
                 {-1.0f, -1.0f}});
            outVertices.push_back({{_bounds.x, _bounds.y + _bounds.height},
                                   color,
                                   {-1.0f, -1.0f}});

            outIndices.push_back(offset + 0);
            outIndices.push_back(offset + 1);
            outIndices.push_back(offset + 2);
            outIndices.push_back(offset + 2);
            outIndices.push_back(offset + 3);
            outIndices.push_back(offset + 0);
        }
    };
} // namespace Engine::UI
