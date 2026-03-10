#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Renderer {
    class Camera {
      public:
        glm::vec3 pos = {0.0f, 0.0f, 10.0f};
        glm::vec3 front = {0.0f, 0.0f, -1.0f};
        glm::vec3 up = {0.0f, 1.0f, 0.0f};

        float yaw = -90.0f;
        float pitch = 0.0f;
        float speed = 5.0f;
        float sensitivity = 0.1f;
        float zoom = 45.0f;

        glm::mat4 GetViewMatrix() {
            return glm::lookAt(pos, pos + front, up);
        }

        void update(float deltaTime);
        void rotate(float xoffset, float yoffset);
        void zoom_input(float yoffset);
    };
} // namespace Engine::Renderer
