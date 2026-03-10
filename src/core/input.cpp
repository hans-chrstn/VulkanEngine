#include "input.hpp"
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>

namespace Engine::Core {
    GLFWwindow *Input::_window = nullptr;

    bool Input::isKeyPressed(int keyCode) {
        auto state = glfwGetKey(_window, keyCode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::isMouseButtonPressed(int button) {
        auto state = glfwGetMouseButton(_window, button);
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::getMousePosition() {
        double xpos;
        double ypos;
        glfwGetCursorPos(_window, &xpos, &ypos);
        return {(float)xpos, (float)ypos};
    }

    float Input::getMouseX() {
        return getMousePosition().x;
    }
    float Input::getMouseY() {
        return getMousePosition().y;
    }

    float Input::scroll_offset = 0.0f;
} // namespace Engine::Core
