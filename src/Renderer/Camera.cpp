#include "Renderer/Camera.hpp"

Camera::Camera(vector position, normal forward, normal up) noexcept :
    m_position{position},
    m_forward{forward},
    m_up{up},
    m_right{glm::normalize(glm::cross(m_forward, m_up))},
    video_mode{glfwGetVideoMode(glfwGetPrimaryMonitor())}
{
    updateView();
    updateProjection();
}

void Camera::move(const vector movement) noexcept
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
        m_position += m_right * x;

    const float y = movement.y;
    if (y != 0.f)
        m_position += m_up * y;

    updateView();
}

void Camera::changeFov(const float delta) noexcept
{
    constexpr float minFov = 1.f;
    constexpr float maxFov = 180.f;

    m_fov = std::clamp(m_fov + delta, minFov, maxFov);

    updateProjection();
}

void Camera::yaw(const float delta) noexcept 
{
    matrix rotation = glm::rotate(
        glm::mat4{1.f},
        glm::radians(delta) * m_sensitivityX * (m_invertX ? 1.f : -1.f),
        m_up
    );

    m_forward = glm::normalize(
        glm::vec3{rotation * glm::vec4{m_forward, 1.f}}
    );

    m_right = glm::normalize(
        glm::cross(m_forward, m_up)
    );

    updateView();
}

void Camera::pitch(const float delta) noexcept 
{
    matrix rotation = glm::rotate(
        glm::mat4{1.f},
        glm::radians(delta) * m_sensitivityY * (m_invertY ? 1.f : -1.f),
        m_right
    );

    m_forward = glm::normalize(
        glm::vec3{rotation * glm::vec4{m_forward, 1.f}}
    );

    m_up = glm::normalize(
        glm::cross(m_right, m_forward)
    );

    updateView();
}

void Camera::roll(const float delta) noexcept 
{
    matrix rotation = glm::rotate(glm::mat4{1.f}, glm::radians(delta), m_forward);

    m_up = glm::normalize(glm::vec3{rotation * glm::vec4{m_up, 1.f}});
    m_right = glm::normalize(glm::cross(m_forward, m_up));

    updateView();
}

void Camera::resetRotation() noexcept
{
    m_forward = glm::vec3{0.f, 0.f, -1.f};
    m_up = glm::vec3{0.f, 1.f, 0.f};
    m_right = glm::vec3{1.f, 0.f, 0.f};

    updateView();
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
        20000.f
    );
}

