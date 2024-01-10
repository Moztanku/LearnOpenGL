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
    float angleX = 0.0f;
    float angleY = 0.0f;

    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
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
        state.mix += amount;
        state.mix = std::clamp(state.mix, 0.0f, 1.0f);
    };
}

StateModifier changeAngleX(const float amount) {
    return [amount](State& state) {
        state.angleX += amount;
    };
}

StateModifier changeAngleY(const float amount) {
    return [amount](State& state) {
        state.angleY += amount;
    };
}

StateModifier accelerate(const glm::vec3& amount) {
    return [amount](State& state) {
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, -state.angleX, glm::vec3(1.f, 0.f, 0.f));
        rotation = glm::rotate(rotation, -state.angleY, glm::vec3(0.f, 1.f, 0.f));

        glm::vec4 directioned_amount = rotation * glm::vec4(amount, 1.f);

        state.posX += directioned_amount.x;
        state.posY += directioned_amount.y;
        state.posZ += directioned_amount.z;

        std::cout << std::format("Pos: {} {} {}", state.posX, state.posY, state.posZ) << std::endl;
    };
}

const std::array<Input, 9> inputs {
    Input{GLFW_KEY_ESCAPE, closeWindow},
    Input{GLFW_KEY_UP, changeMix(0.01f)},
    Input{GLFW_KEY_DOWN, changeMix(-0.01f)},
    Input{GLFW_KEY_W, changeAngleX(0.01f)},
    Input{GLFW_KEY_S, changeAngleX(-0.01f)},
    Input{GLFW_KEY_A, changeAngleY(0.01f)},
    Input{GLFW_KEY_D, changeAngleY(-0.01f)},
    Input{GLFW_KEY_SPACE, accelerate(glm::vec3{0.f, 0.f, 0.01f})},
    Input{GLFW_KEY_LEFT_SHIFT, accelerate(glm::vec3{0.f, 0.f, -0.01f})}
};


void process_input(GLFWwindow* window, State& state)
{
    for (const auto& input : inputs)
    {
        if(glfwGetKey(window, input.glfw_key) == GLFW_PRESS)
            input.modifier(state);
    }
}