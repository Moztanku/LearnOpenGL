#pragma once

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include <array>
#include <algorithm>
#include <functional>
#include <iostream>
#include <format>

struct State 
{
    float mix = 0.2f;
    float Δt = 0.0f;

    // Camera
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float pitch = 0.0f;
    float yaw = 0.0f;

    static float& fov() {
        static float fov = 45.0f;
        return fov;
    };
};

using StateModifier = std::function<void(State&)>;

struct Input {
    int glfw_key = GLFW_KEY_UNKNOWN;
    StateModifier modifier = nullptr;
};

void closeWindow(State& state)
{
    glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
}

StateModifier changeMix(const float amount) {
    return [amount](State& state) {
        state.mix += amount * state.Δt;
        state.mix = std::clamp(state.mix, 0.0f, 1.0f);
    };
}

StateModifier moveForward(const float amount) {
    return [amount](State& state) {
        state.cameraPos += state.cameraFront * amount * state.Δt;
    };
}

StateModifier strafe(const float amount) {
    return [amount](State& state) {
        state.cameraPos += glm::normalize(glm::cross(state.cameraFront, state.cameraUp)) * amount * state.Δt;
    };
}

void toggleWireFrame(State& state)
{
    static bool wireframe = false;
    wireframe = !wireframe;
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
}

void resetCamera(State& state)
{
    state.cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    state.cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    state.cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    state.pitch = 0.0f;
    state.yaw = -90.0f;
    state.fov() = 45.0f;
}

const std::array<Input, 9> inputs_each_frame {
    Input{GLFW_KEY_ESCAPE, closeWindow},
    Input{GLFW_KEY_UP, changeMix(0.5f)},
    Input{GLFW_KEY_DOWN, changeMix(-0.5f)},
    Input{GLFW_KEY_W, moveForward(1.0f)},
    Input{GLFW_KEY_S, moveForward(-1.0f)},
    Input{GLFW_KEY_A, strafe(-1.0f)},
    Input{GLFW_KEY_D, strafe(1.0f)}
    // Input{GLFW_KEY_SPACE, accelerate(glm::vec3{0.f, 0.f, 0.01f})},
    // Input{GLFW_KEY_LEFT_SHIFT, accelerate(glm::vec3{0.f, 0.f, -0.01f})}
};

const std::array<Input, 2> inputs_on_release {
    Input{GLFW_KEY_TAB, toggleWireFrame},
    Input{GLFW_KEY_R, resetCamera}
};


void process_input(GLFWwindow* window, State& state)
{
    static float last_time = glfwGetTime();
    float current_time = glfwGetTime();

    state.Δt = current_time - last_time;

    last_time = current_time;

    for (const auto& input : inputs_each_frame)
    {
        if(glfwGetKey(window, input.glfw_key) == GLFW_PRESS)
            input.modifier(state);
    }

    static int last_key = GLFW_KEY_UNKNOWN;
    for (const auto& input : inputs_on_release)
    {
        if(glfwGetKey(window, input.glfw_key) == GLFW_PRESS)
            last_key = input.glfw_key;

        if(glfwGetKey(window, input.glfw_key) == GLFW_RELEASE && last_key == input.glfw_key)
        {
            input.modifier(state);
            last_key = GLFW_KEY_UNKNOWN;
        }
    }

    static double last_x, last_y = -1.0;
    double new_x, new_y;

    glfwGetCursorPos(window, &new_x, &new_y);

    double xΔ = new_x - last_x;
    double yΔ = new_y - last_y;

    last_x = new_x;
    last_y = new_y;

    constexpr float sensitivity = 0.1f;

    constexpr bool invert_x = false;
    constexpr bool invert_y = false;

    if (xΔ != 0.0 || yΔ != 0.0)
    {
        state.yaw += xΔ * sensitivity * (invert_x ? -1.0f : 1.0f);
        state.pitch += yΔ * sensitivity * (invert_y ? 1.0f : -1.0f);

        state.yaw = std::fmod(state.yaw, 360.0f);
        state.pitch = std::clamp(state.pitch, -89.0f, 89.0f);

        glm::vec3 dir;
        dir.x = cos(glm::radians(state.yaw)) * cos(glm::radians(state.pitch));
        dir.y = sin(glm::radians(state.pitch));
        dir.z = sin(glm::radians(state.yaw)) * cos(glm::radians(state.pitch));

        state.cameraFront = glm::normalize(dir);
    }
}