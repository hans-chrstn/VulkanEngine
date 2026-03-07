#include "window.hpp"
#include "logger.hpp"
#include <GLFW/glfw3.h>

namespace Engine::Core {
    Window::Window(int width, int height, const std::string &title)
        : m_Width(width), m_Height(height), m_Title(title) {
        if (!glfwInit()) {
            ENGINE_FATAL("Failed to initialize GLFW!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr,
                                    nullptr);

        if (m_Window == nullptr) {
            ENGINE_FATAL("Failed to create a window");
        }
    }

    Window::~Window() {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_Window);
    }

    bool Window::pollEvents() const {
        glfwPollEvents();
        return true;
    }
} // namespace Engine::Core
