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

    inline static State* state_ptr = nullptr;
    inline static MouseScrollHandler scrollHandler = nullptr;

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

            switch (state)
            {
                case KeyState::IDLE: {
                    if (isPressed)
                        state = KeyState::PRESSED;

                    break;
                }
                case KeyState::PRESSED: {
                    const auto& handler = m_keyHandlers[key].pressed;
                    if (handler)
                        handler(m_state, delta);

                    if (isPressed)
                        state = KeyState::HELD;
                    else
                        state = KeyState::RELEASED;

                    break;
                }
                case KeyState::HELD: {
                    const auto& handler = m_keyHandlers[key].held;
                    if (handler)
                        handler(m_state, delta);

                    if (!isPressed)
                        state = KeyState::RELEASED;

                    break;
                }
                case KeyState::RELEASED: {
                    const auto& handler = m_keyHandlers[key].released;
                    if (handler)
                        handler(m_state, delta);

                    if (isPressed)
                        state = KeyState::PRESSED;
                    else
                        state = KeyState::IDLE;
                    break;
                }
            }
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
