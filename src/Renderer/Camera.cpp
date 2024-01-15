#include "Renderer/Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up) noexcept :
    m_position{position},
    m_forward{forward},
    m_up{up},
    video_mode{glfwGetVideoMode(glfwGetPrimaryMonitor())}
{
    updateView();
    updateProjection();
}

void Camera::move(const glm::vec3 movement) noexcept
{
    constexpr bool FREECAM_MODE = true;

    const float z = movement.z;
    if (z != 0.f) {
        if constexpr (FREECAM_MODE)
            m_position += m_forward * z;
        else
            m_position += glm::normalize(
                glm::vec3{m_forward.x, 0.f, m_forward.z}
            ) * z;
    }

    const float x = movement.x;
    if (x != 0.f)
        m_position += glm::normalize(
            glm::cross(m_forward, m_up)
        ) * x;

    const float y = movement.y;
    if (y != 0.f)
        m_position += m_up * y;

    updateView();
}

void Camera::rotate(const glm::vec2 rotation) noexcept
{
    m_yaw += rotation.x * m_sensitivityX * (m_invertX ? -1.f : 1.f);
    m_pitch += rotation.y * m_sensitivityY * (m_invertY ? 1.f : -1.f);

    m_yaw = std::fmod(m_yaw, 360.f);
    m_pitch = std::clamp(m_pitch, -89.f,89.f );

    glm::vec3 dir;
    dir.x = cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
    dir.y = sinf(glm::radians(m_pitch));
    dir.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));

    m_forward = glm::normalize(dir);

    updateView();
}

void Camera::changeFov(const float delta) noexcept
{
    constexpr float minFov = 1.f;
    constexpr float maxFov = 180.f;

    m_fov = std::clamp(m_fov + delta, minFov, maxFov);

    updateProjection();
}

// Private

void Camera::updateView() noexcept
{
    m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
}

void Camera::updateProjection() noexcept
{
    m_projection = glm::perspective(
        glm::radians(m_fov),
        static_cast<float>(video_mode->width) / static_cast<float>(video_mode->height),
        0.1f,
        100.f
    );
}

