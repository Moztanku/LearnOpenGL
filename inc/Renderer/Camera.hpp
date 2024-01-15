#pragma once

#include <cmath>
#include <tuple>
#include <algorithm>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
    public:
        Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up) noexcept;
        ~Camera() = default;


        void move(const glm::vec3 movement) noexcept;
        void rotate(const glm::vec2 rotation) noexcept;
        void changeFov(const float delta) noexcept;

        void rotate(const glm::vec3 rotation) noexcept
{
    m_yaw += rotation.x * m_sensitivityX * (m_invertX ? -1.f : 1.f);
    m_pitch += rotation.y * m_sensitivityY * (m_invertY ? 1.f : -1.f);
    m_roll += rotation.z;

    m_yaw = std::fmod(m_yaw, 360.f);
    m_pitch = std::clamp(m_pitch, -89.f,89.f );
    m_roll = std::fmod(m_roll, 360.f);

    glm::vec3 dir;
    dir.x = cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
    dir.y = sinf(glm::radians(m_pitch));
    dir.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));

    m_forward = glm::normalize(dir);

    updateView();
}

        [[nodiscard]] inline const glm::mat4& getView() const noexcept { return m_view; }
        [[nodiscard]] inline const glm::mat4& getProjection() const noexcept { return m_projection; }
        [[nodiscard]] inline const glm::vec2 getRotation() const noexcept { return {m_yaw, m_pitch}; }
        [[nodiscard]] inline const glm::vec2 getSensitivity() const noexcept { return {m_sensitivityX, m_sensitivityY}; }
        [[nodiscard]] inline const glm::vec3 getPosition() const noexcept { return m_position; }
        [[nodiscard]] inline const glm::vec3 getForward() const noexcept { return m_forward; }
        [[nodiscard]] inline const glm::vec3 getUp() const noexcept { return m_up; }

    private:
        glm::vec3 m_position;
        glm::vec3 m_forward;
        glm::vec3 m_up;

        glm::mat4 m_view;
        glm::mat4 m_projection;

        float m_yaw = -90.f;
        float m_pitch = 0.f;
        float m_roll = 0.f;

        float m_fov = 75.f;

        bool m_invertX = false;
        bool m_invertY = false;
        float m_sensitivityX = 0.1f;
        float m_sensitivityY = 0.1f;

        const GLFWvidmode* video_mode;

        void updateView() noexcept;

        void updateProjection() noexcept;
};