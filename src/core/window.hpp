#pragma once
#include <GLFW/glfw3.h>
#include <string>

namespace Engine::Core {
    class Window {
      private:
        static void FramebufferResizeCallback(GLFWwindow *window, int width,
                                              int height);

        int m_Width;
        int m_Height;
        std::string m_Title;
        GLFWwindow *m_Window;

      public:
        Window(int width, int height, const std::string &title);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        bool shouldClose() const;
        bool pollEvents() const;

        GLFWwindow *getNativeWindow() const {
            return m_Window;
        }
    };
} // namespace Engine::Core
