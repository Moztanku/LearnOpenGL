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
            vector position = {0.f, 0.f, 0.f},
            normal forward = {0.f, 0.f, -1.f},
            normal up = {0.f, 1.f, 0.f}
        ) noexcept;
        ~Camera() = default;

        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;
        auto operator=(const Camera&) -> Camera& = delete;
        auto operator=(Camera&&) -> Camera& = delete;

        auto move(const vector movement) noexcept -> void;
        auto changeFov(const angle delta) noexcept -> void;

        auto yaw(const angle delta) noexcept -> void;
        auto pitch(const angle delta) noexcept -> void;
        auto roll(const angle delta) noexcept -> void;
        auto resetRotation() noexcept -> void;

        [[nodiscard]] inline auto getView() const noexcept -> const matrix& { return m_view; }
        [[nodiscard]] inline auto getProjection() const noexcept -> const matrix& { return m_projection; }

        [[nodiscard]] inline auto getPosition() const noexcept -> const vector& { return m_position; }
        [[nodiscard]] inline auto getForward() const noexcept -> const normal& { return m_forward; }
        [[nodiscard]] inline auto getUp() const noexcept -> const normal& { return m_up; }
        [[nodiscard]] inline auto getRight() const noexcept -> const normal& { return m_right; }
    private:
        vector m_position{};

        normal m_forward{};
        normal m_up{};
        normal m_right{};

        matrix m_view{};
        matrix m_projection{};

        angle m_fov = 75.f;

        const GLFWvidmode* video_mode;

        auto updateView() noexcept -> void;
        auto updateProjection() noexcept -> void;
}; // class Camera

} // namespace Renderer
