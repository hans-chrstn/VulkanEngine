#pragma once
#include "vertex.hpp"
#include <vector>

namespace Engine::UI {
    struct Rect {
        float x;
        float y;
        float width;
        float height;

        bool contains(float px, float py) const {
            return px >= x && px <= (x + width) && py >= y &&
                   py <= (y + height);
        }
    };

    class Widget {
      protected:
        Rect _bounds;
        bool _visible = true;
        bool _isHovered = false;

      public:
        virtual ~Widget() = default;
        virtual void onUpdate(float deltaTime) = 0;
        virtual void onRender(std::vector<UIVertex> &outVertices,
                              std::vector<uint32_t> &outIndices) = 0;
    };
} // namespace Engine::UI
