#pragma once
#include "renderer/font.hpp"
#include "widget.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Engine::UI {
    class UIText : public Widget {
      private:
        std::string _text;
        glm::vec4 _color;
        float _scale;
        const Renderer::Font &_font;

      public:
        UIText(const std::string &text, Rect bounds, glm::vec4 color,
               float scale, const Renderer::Font &font)
            : _text(text), _color(color), _scale(scale), _font(font) {
            _bounds = bounds;
        }

        void onUpdate(float deltaTime) override {}

        void onRender(std::vector<UIVertex> &outVertices,
                      std::vector<uint32_t> &outIndices) override {
            float x = _bounds.x;
            float y = _bounds.y + (_bounds.height * 0.7f);

            for (char c : _text) {
                const auto &ch = _font.getCharacter(c);

                float xpos = x + ch.bearing.x * _scale;
                float ypos = y - ch.bearing.y * _scale;

                float w = ch.size.x * _scale;
                float h = ch.size.y * _scale;

                uint32_t offset = static_cast<uint32_t>(outVertices.size());

                outVertices.push_back(
                    {{xpos, ypos}, _color, {ch.texOffset.x, ch.texOffset.y}});
                outVertices.push_back(
                    {{xpos + w, ypos},
                     _color,
                     {ch.texOffset.x + ch.texSize.x, ch.texOffset.y}});
                outVertices.push_back({{xpos + w, ypos + h},
                                       _color,
                                       {ch.texOffset.x + ch.texSize.x,
                                        ch.texOffset.y + ch.texSize.y}});
                outVertices.push_back(
                    {{xpos, ypos + h},
                     _color,
                     {ch.texOffset.x, ch.texOffset.y + ch.texSize.y}});

                outIndices.push_back(offset + 0);
                outIndices.push_back(offset + 1);
                outIndices.push_back(offset + 2);
                outIndices.push_back(offset + 2);
                outIndices.push_back(offset + 3);
                outIndices.push_back(offset + 0);

                x += (ch.advance >> 6) * _scale;
            }
        }
    };
} // namespace Engine::UI
