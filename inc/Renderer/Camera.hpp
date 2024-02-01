#pragma once

#include <cmath>
#include <tuple>
#include <algorithm>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Renderer 
{
    
    class Camera
    {
            using vector = glm::vec3;
            using normal = glm::vec3;
            using angle = glm::float32;
            using matrix = glm::mat4;
        public:
            Camera(
                vector position = {0.f,0.f,0.f},
                normal forward = {0.f,0.f,-1.f},
                normal up = {0.f,1.f,0.f}
            ) noexcept;
            ~Camera() = default;


            void move(const vector movement) noexcept;
            void changeFov(const angle delta) noexcept;

            void yaw(const angle delta) noexcept;
            void pitch(const angle delta) noexcept;
            void roll(const angle delta) noexcept;
            void resetRotation() noexcept;

            [[nodiscard]] inline const matrix& getView() const noexcept { return m_view; }
            [[nodiscard]] inline const matrix& getProjection() const noexcept { return m_projection; }

            [[nodiscard]] inline const vector getPosition() const noexcept { return m_position; }
            [[nodiscard]] inline const normal getForward() const noexcept { return m_forward; }
            [[nodiscard]] inline const normal getUp() const noexcept { return m_up; }
            [[nodiscard]] inline const normal getRight() const noexcept { return m_right; }
        private:
            vector m_position;

            normal m_forward;
            normal m_up;
            normal m_right;

            matrix m_view;
            matrix m_projection;

            angle m_fov = 75.f;

            const GLFWvidmode* video_mode;

            void updateView() noexcept;
            void updateProjection() noexcept;

    }; // class Camera

} // namespace Renderer
