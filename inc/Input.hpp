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

    inline static State* state_ptr = nullptr;
    inline static MouseScrollHandler scrollHandler = nullptr;
    inline static MouseButtonHandler mouseButtonHandler = nullptr;

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

        void update() noexcept
        {
            glfwPollEvents();

            handleKeyboard(getDelta());

            if (m_mouseHandler)
            {
                double xpos, ypos;
                glfwGetCursorPos(m_window, &xpos, &ypos);

                m_mouseHandler(
                    m_state,
                    getMouseDelta({xpos, ypos}),
                    {xpos, ypos}
                );
            }
        }

        void setKeyHandler(const Key key, const KeyHandler& handler) noexcept
        {
            m_keyHandlers[key] = handler;

            if (handler.pressed || handler.held || handler.released)
                m_keys.emplace_back(key, KeyState::IDLE);
        }

        void setMouseHandler(const MouseHandler& handler) noexcept
        {
            m_mouseHandler = handler;
        }

        void setMouseButtonHandler(const MouseButtonHandler handler) noexcept
        {
            mouseButtonHandler = handler;

            const auto mb_callback = [](GLFWwindow* window, int button, int action, int mods)
            {
                const bool isPressed =
                    action == GLFW_PRESS;

                setMouseButtonHandler(
                    *state_ptr,
                    button,
                    isPressed
                );
            };

            glfwSetMouseButtonCallback(m_window, mb_callback);
        }

        void setMouseScrollHandler(const MouseScrollHandler& handler) noexcept
        {
            scrollHandler = handler;

            const auto scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset)
            {
                scrollHandler(
                    *state_ptr, 
                    static_cast<float>(yoffset)
                );
            };

            glfwSetScrollCallback(m_window, scroll_callback);
        }

        void reset() noexcept
        {
            m_keys.clear();
            m_keyHandlers.clear();
            m_mouseHandler = nullptr;

            scrollHandler = nullptr;
            mouseButtonHandler = nullptr;

            glfwSetScrollCallback(m_window, nullptr);
        }
    private:
        GLFWwindow* m_window;
        State& m_state;

        MouseHandler m_mouseHandler{nullptr};

        enum class KeyState {
            IDLE,
            PRESSED,
            HELD,
            RELEASED
        };

        std::vector<std::pair<Key, KeyState>> m_keys{};
        std::unordered_map<Key, KeyHandler> m_keyHandlers{};

        void handleKeyboard(const float delta) noexcept
        {
            for (auto& [key, state] : m_keys)
            {
                handleKey(key, state, delta);
            }
        }

        void handleKey(const Key key, KeyState& state, const float delta) noexcept
        {
            const bool isPressed = glfwGetKey(m_window, key) == GLFW_PRESS;

            const auto& handler =
                (state == KeyState::PRESSED) ? m_keyHandlers[key].pressed :
                (state == KeyState::HELD) ? m_keyHandlers[key].held :
                (state == KeyState::RELEASED) ? m_keyHandlers[key].released :
                nullptr;

            if (handler)
                handler(m_state, delta);

            if (isPressed)
                state =
                    (state == KeyState::IDLE) ? KeyState::PRESSED :
                    (state == KeyState::PRESSED) ? KeyState::HELD :
                    (state == KeyState::RELEASED) ? KeyState::PRESSED :
                    state;
            else
                state =
                    (state == KeyState::PRESSED) ? KeyState::RELEASED :
                    (state == KeyState::HELD) ? KeyState::RELEASED :
                    (state == KeyState::RELEASED) ? KeyState::IDLE :
                    state;
        }

        void setMouseInputMode(int input_mode) noexcept
        {
            glfwSetInputMode(m_window, GLFW_CURSOR, input_mode);
        }

        static glm::vec2 getMouseDelta(const glm::vec2 mousePos) noexcept
        {
            static glm::vec2 last_mousePos{0.f, 0.f};

            const glm::vec2 mouseDelta = mousePos - last_mousePos;
            last_mousePos = mousePos;

            return mouseDelta;
        }

        static float getDelta() noexcept
        {
            static float last_time = glfwGetTime();
            const float current_time = glfwGetTime();

            const float delta = current_time - last_time;
            last_time = current_time;

            return delta;
        }
};  // class Input
