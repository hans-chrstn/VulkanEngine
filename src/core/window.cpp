#include "window.hpp"
#include "logger.hpp"
#include <GLFW/glfw3.h>

namespace Engine::Core {
    Window::Window(int width, int height, const std::string &title)
        : m_Width(width), m_Height(height), m_Title(title) {
        if (!glfwInit()) {
            ENGINE_FATAL("Failed to initialize GLFW!");
        }
    }
} // namespace Engine::Core
