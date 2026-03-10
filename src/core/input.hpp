#pragma once
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

namespace Engine::Core {
    class Input {
      private:
        static GLFWwindow *_window;

      public:
        static void setWindow(GLFWwindow *window) {
            _window = window;
        }

        static bool isKeyPressed(int keyCode);

        static bool isMouseButtonPressed(int button);
        static glm::vec2 getMousePosition();
        static float getMouseX();
        static float getMouseY();

        static float scroll_offset;
        static void scrollCallback(GLFWwindow *window, double xoffset,
                                   double yoffset) {
            scroll_offset = float(yoffset);
        }
    };
} // namespace Engine::Core
