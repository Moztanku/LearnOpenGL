#pragma once

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

#include <array>
#include <algorithm>
#include <functional>
#include <iostream>
#include <format>

#include "Renderer/Camera.hpp"

struct State 
{
    float mix = 0.2f;
    float Δt = 0.0f;

    Camera camera{
        glm::vec3{0.f,0.f,3.f},
        glm::vec3{0.f,0.f,-1.f},
        glm::vec3{0.f,1.f,0.f}
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
        const glm::vec3 move{
            0.f,
            0.f,
            amount * state.Δt
        };

        state.camera.move(move);
    };
}

StateModifier moveUp(const float amount) {
    return [amount](State& state) {
        const glm::vec3 move{
            0.f,
            amount * state.Δt,
            0.f
        };

        state.camera.move(move);
    };
}

StateModifier strafe(const float amount) {
    return [amount](State& state) {
        const glm::vec3 move{
            amount * state.Δt,
            0.f,
            0.f
        };

        state.camera.move(move);
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
    state.camera.reset();
}

const std::array<Input, 10> inputs_each_frame {
    Input{GLFW_KEY_ESCAPE, closeWindow},
    Input{GLFW_KEY_UP, changeMix(.5f)},
    Input{GLFW_KEY_DOWN, changeMix(-.5f)},
    Input{GLFW_KEY_W, moveForward(1.f)},
    Input{GLFW_KEY_S, moveForward(-1.f)},
    Input{GLFW_KEY_A, strafe(-1.f)},
    Input{GLFW_KEY_D, strafe(1.f)},
    Input{GLFW_KEY_SPACE, moveUp(1.f)},
    Input{GLFW_KEY_LEFT_SHIFT, moveUp(-1.f)}
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

    if (xΔ != 0.0 || yΔ != 0.0)
    {
        state.camera.rotate({xΔ, yΔ});
    }
}