#include "camera.hpp"
#include "core/input.hpp"
#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace Engine::Renderer {
    void Camera::update(float deltaTime) {
        float velocity = speed * deltaTime;

        if (Core::Input::isKeyPressed(GLFW_KEY_W)) {
            pos += front * velocity;
        }

        if (Core::Input::isKeyPressed(GLFW_KEY_S)) {
            pos -= front * velocity;
        }

        if (Core::Input::isKeyPressed(GLFW_KEY_A)) {
            pos -= glm::normalize(glm::cross(front, up)) * velocity;
        }

        if (Core::Input::isKeyPressed(GLFW_KEY_D)) {
            pos += glm::normalize(glm::cross(front, up)) * velocity;
        }
    }

    void Camera::rotate(float xoffset, float yoffset) {
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        // prevent flip duh
        if (pitch > 89.0f) {
            pitch = 89.0f;
        }
        if (pitch < -89.0f) {
            pitch = -89.0f;
        }

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);
    }

    void Camera::zoom_input(float yoffset) {
        zoom -= yoffset;
        if (zoom < 1.0f) {
            zoom = 1.0f;
        }
        if (zoom > 45.0f) {
            zoom = 45.0f;
        }
    }
} // namespace Engine::Renderer
