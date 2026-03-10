#include "layout_manager.hpp"

namespace Engine::UI {
    Rect LayoutManager::GetPanelRect(PanelSide side, int width, int height) {
        float padding = 2.0f;
        float leftWidth = width * 0.20f;
        float rightWidth = width * 0.25f;
        float topHeight = 30.0f;

        float remainingHeight = (float)height - topHeight;

        switch (side) {
        case PanelSide::Top:
            return {0, 0, (float)width, topHeight};

        case PanelSide::Left:
            return {padding, topHeight + padding, leftWidth - (padding * 2),
                    remainingHeight * 0.7f - (padding * 2)};

        case PanelSide::Bottom:
            return {padding, topHeight + (remainingHeight * 0.7f) + padding,
                    leftWidth - (padding * 2),
                    remainingHeight * 0.3f - padding};

        case PanelSide::Right:
            return {(float)width - rightWidth + padding, topHeight + padding,
                    rightWidth - (padding * 2),
                    remainingHeight - (padding * 2)};

        case PanelSide::Center:
            return {leftWidth, topHeight, (float)width - leftWidth - rightWidth,
                    remainingHeight};

        default:
            return {0, 0, 0, 0};
        }
    }
} // namespace Engine::UI
