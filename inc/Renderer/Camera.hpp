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
        Camera(glm::vec3 position, glm::vec3 forward, glm::vec3 up) :
            initial_position{position},
            initial_forward{forward},
            initial_up{up},
            video_mode{glfwGetVideoMode(glfwGetPrimaryMonitor())}
        {
            reset();
        }

        ~Camera() = default;

        const glm::mat4& getView()
        {
            return m_view;
        }

        const glm::mat4& getProjection()
        {
            return m_projection;
        }

        void move(const glm::vec3 movement)
        {
            const float z = movement.z;

            if (z != 0.f)
                m_position += m_forward * z;

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

        void rotate(const glm::vec2 rotation)
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

        void changeFov(const float delta)
        {
            constexpr float minFov = 1.f;
            constexpr float maxFov = 180.f;

            m_fov = std::clamp(m_fov + delta, minFov, maxFov);

            m_projection = glm::perspective(
                glm::radians(m_fov),
                static_cast<float>(video_mode->width) / static_cast<float>(video_mode->height),
                0.1f,
                100.f
            );

            updateProjection();
        }

        void reset()
        {
            m_position = initial_position;
            m_forward = initial_forward;
            m_up = initial_up;

            updateView();
        
            m_pitch = 0.f;
            m_yaw = 0.f;
            m_fov = 75.f;

            updateProjection();
        }

    private:
        glm::vec3 m_position;
        glm::vec3 m_forward;
        glm::vec3 m_up;

        glm::mat4 m_view;
        glm::mat4 m_projection;

        float m_pitch = 0.f;
        float m_yaw = 0.f;
        float m_fov = 75.f;

        bool m_invertX = false;
        bool m_invertY = false;
        float m_sensitivityX = 0.1f;
        float m_sensitivityY = 0.1f;

        const glm::vec3 initial_position;
        const glm::vec3 initial_forward;
        const glm::vec3 initial_up;

        const GLFWvidmode* video_mode;


        void updateView() {
            m_view = glm::lookAt(
                m_position,
                m_position + m_forward,
                m_up
            );
        }

        void updateProjection() {
            m_projection = glm::perspective(
                glm::radians(m_fov),
                static_cast<float>(video_mode->width) / static_cast<float>(video_mode->height),
                0.1f,
                100.f
            );
        }
};