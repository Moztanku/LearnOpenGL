#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include "Renderer/Camera.hpp"

#include "jac/type_defs.hpp"

template <typename State>
class Input
{
    using Key = int;
    using StateModifier = std::function<void(State&, const float)>;
    using MouseHandler = std::function<void(State&, const glm::vec2, const glm::vec2)>;
    using MouseScrollHandler = std::function<void(State&, const float)>;
    using MouseButtonHandler = std::function<void(State&, const int, const bool)>;

    inline static State* state_ptr = nullptr;   // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - needed for GLFW callbacks
    inline static MouseScrollHandler scrollHandler = nullptr;  // NOLINT ^
    inline static MouseButtonHandler mouseButtonHandler = nullptr;  // NOLINT ^

    public:
        struct KeyHandler {
            StateModifier pressed{nullptr};
            StateModifier held{nullptr};
            StateModifier released{nullptr};
        };

        Input(GLFWwindow* window, State& state) noexcept :
            m_window{window},
            m_state{state}
        {
            state_ptr = &m_state;
            setMouseInputMode(GLFW_CURSOR_DISABLED);
        }

        ~Input() noexcept
        {
            reset();
            state_ptr = nullptr;
        };

        Input(const Input&) = delete;
        Input(Input&&) = delete;
        auto operator=(const Input&) -> Input& = delete;
        auto operator=(Input&&) -> Input& = delete;
        
        auto update() noexcept -> void;

        auto setKeyHandler(const Key key, const KeyHandler& handler) noexcept -> void;
        auto setMouseHandler(const MouseHandler& handler) noexcept -> void;
        auto setMouseButtonHandler(const MouseButtonHandler handler) noexcept -> void;
        auto setMouseScrollHandler(const MouseScrollHandler& handler) noexcept -> void;

        auto reset() noexcept -> void;
    private:
        GLFWwindow* m_window;
        State& m_state; // NOLINT (cppcoreguidelines-avoid-const-or-ref-data-members) - maybe change to shared_ptr

        MouseHandler m_mouseHandler{nullptr};

        enum class KeyState {
            IDLE,
            PRESSED,
            HELD,
            RELEASED
        };

        std::vector<std::pair<Key, KeyState>> m_keys{};
        std::unordered_map<Key, KeyHandler> m_keyHandlers{};

        auto handleKeyboard(const float delta) noexcept -> void;
        auto handleKey(const Key key, KeyState& state, const float delta) noexcept -> void;

        auto setMouseInputMode(int input_mode) noexcept -> void;

        static auto getMouseDelta(const glm::vec2 mousePos) noexcept -> glm::vec2;
        static auto getDelta() noexcept -> float;
};  // class Input

#define INCLUDE_INPUT_INL
#include "Input.inl"
#undef INCLUDE_INPUT_INL
