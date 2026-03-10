#pragma once
#include "widget.hpp"
#include <string>

namespace Engine::UI {
    enum class PanelSide { Left, Right, Top, Bottom, Center };
    struct Panel {
        std::string name;
        PanelSide side;
        float widthPercent;
        float heightPercent;
    };

    class LayoutManager {
      public:
        static Rect GetPanelRect(PanelSide side, int width, int height);
    };
} // namespace Engine::UI
