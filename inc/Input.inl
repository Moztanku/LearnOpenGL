#ifndef INCLUDE_INPUT_INL
    #error "File should not be included directly, include Input.hpp instead."
#else
#include "Input.hpp"

namespace {
    template<typename State>
    static State* state_ptr = nullptr;   // NOLINT (cppcoreguidelines-avoid-non-const-global-variables) - needed for GLFW callbacks

    template<typename State>
    static Input<State>::MouseScrollHandler scrollHandler = nullptr;

    template <typename State>
    static Input<State>::MouseButtonHandler mouseButtonHandler = nullptr;
}   // namespace

// Ctors & Dtors //

template <typename State>
Input<State>::Input(GLFWwindow* window, State& state) noexcept :
    m_window{window},
    m_state{state}
{
    state_ptr<State> = &m_state;
    setMouseInputMode(GLFW_CURSOR_DISABLED);
}

template <typename State>
Input<State>::~Input() noexcept
{
    reset();
    state_ptr<State> = nullptr;
};

// Ctors & Dtors //
// Public methods //

template <typename State>
auto Input<State>::update() noexcept -> void
{
    glfwPollEvents();

    handleKeyboard(getDelta());

    if (m_mouseHandler)
    {
        double xpos{}, ypos{};
        glfwGetCursorPos(m_window, &xpos, &ypos);

        m_mouseHandler(
            m_state,
            getMouseDelta({xpos, ypos}),
            {xpos, ypos}
        );
    }
}

template <typename State>
auto Input<State>::setKeyHandler(const Key key, const KeyHandler& handler) noexcept -> void
{
    m_keyHandlers[key] = handler;

    if (handler.pressed || handler.held || handler.released)
        m_keys.emplace_back(key, KeyState::IDLE);
}

template <typename State>
auto Input<State>::setMouseHandler(const MouseHandler& handler) noexcept -> void
{
    m_mouseHandler = handler;
}

template <typename State>
auto Input<State>::setMouseButtonHandler(const MouseButtonHandler handler) noexcept -> void
{
    mouseButtonHandler<State> = handler;

    const auto mb_callback = [](GLFWwindow* /*window*/, int button, int action, int /*mods*/)
    {
        const bool isPressed =
            action == GLFW_PRESS;

        mouseButtonHandler<State>(
            *state_ptr<State>,
            button,
            isPressed
        );
    };

    glfwSetMouseButtonCallback(m_window, mb_callback);
}

template <typename State>
auto Input<State>::setMouseScrollHandler(const MouseScrollHandler& handler) noexcept -> void
{
    scrollHandler<State> = handler;

    const auto scroll_callback = [](GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
    {
        scrollHandler<State>(
            *state_ptr<State>, 
            static_cast<float>(yoffset)
        );
    };

    glfwSetScrollCallback(m_window, scroll_callback);
}

template <typename State>
auto Input<State>::reset() noexcept -> void
{
    m_keys.clear();
    m_keyHandlers.clear();
    m_mouseHandler = nullptr;

    scrollHandler<State> = nullptr;
    mouseButtonHandler<State> = nullptr;

    glfwSetScrollCallback(m_window, nullptr);
}

// Public methods //
// Private methods //

template <typename State>
auto Input<State>::handleKeyboard(const float delta) noexcept -> void
{
    for (auto& [key, state] : m_keys)
    {
        handleKey(key, state, delta);
    }
}

template <typename State>
auto Input<State>::handleKey(const Key key, KeyState& state, const float delta) noexcept -> void
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

template <typename State>
auto Input<State>::setMouseInputMode(int input_mode) noexcept -> void
{
    glfwSetInputMode(m_window, GLFW_CURSOR, input_mode);
}

// Private methods //
// Static methods //

template <typename State>
auto Input<State>::getMouseDelta(const glm::vec2 mousePos) noexcept -> glm::vec2
{
    static glm::vec2 last_mousePos{0.f, 0.f};

    const glm::vec2 mouseDelta = mousePos - last_mousePos;
    last_mousePos = mousePos;

    return mouseDelta;
}

template <typename State>
auto Input<State>::getDelta() noexcept -> float
{
    static float last_time = glfwGetTime();
    const float current_time = glfwGetTime();

    const float delta = current_time - last_time;
    last_time = current_time;

    return delta;
}

// Static methods //

#endif // INCLUDE_INPUT_INL
