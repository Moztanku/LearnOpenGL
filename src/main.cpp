/**
 * @file main.cpp
 * @author Moztanku (mostankpl@gmail.com)
 * @brief Entry point of the program, contains main.hpp that wraps main function
 *      and calls run function from main.cpp
 * @version 0.1
 * @date 2024-01-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <format>
#include <memory>
#include <fstream>
#include <iostream>

#include "Input.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/GPU/Shader.hpp"
#include "Renderer/GPU/VertexArray.hpp"
#include "Renderer/GPU/VertexBuffer.hpp"
#include "Renderer/GPU/IndexBuffer.hpp"
#include "Renderer/GPU/VertexBufferLayout.hpp"
#include "Renderer/GPU/Texture.hpp"

#include "jac/main.hpp"
#include "jac/type_defs.hpp"

#if !defined(OpenGL_VERSION_MAJOR) || !defined(OpenGL_VERSION_MINOR)
    constexpr int OpenGL_VERSION_MAJOR = 3;
    constexpr int OpenGL_VERSION_MINOR = 3;
#endif

using Renderer::GPU::Shader;
using Renderer::GPU::Texture;
using Renderer::GPU::VertexArray;
using Renderer::GPU::VertexBuffer;
using Renderer::GPU::VertexBufferLayout;

namespace Resources {
    namespace Shaders {
        const std::filesystem::path basic_vertex = "res/shaders/basic.vert";
        const std::filesystem::path basic_fragment = "res/shaders/basic.frag";
    }

    namespace Textures {
        const std::filesystem::path container = "res/textures/container.png";
        const std::filesystem::path face = "res/textures/grandfather-face.png";
    }
}   // namespace Resources

auto initialize_glfw() -> void;
auto create_window(const uint width, const uint height, const std::string& title) -> unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>;

struct Model {
    std::vector<float> vertices;
    VertexBufferLayout layout;
};

auto read_file(const std::filesystem::path& path) -> Model
{
    Model data;

    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << path << std::endl;
        return data;
    }

    std::string s;
    while (file) {
        std::getline(file, s);

        if (s.contains("float")){
            const uint count = std::stoi(
                s.substr(s.find(' '), s.size() - s.find(' '))
            );
            data.layout.Push<float>(count);
        }
        else
            data.vertices.push_back(std::stof(s));

    }

    return data;
}

struct Box {
    glm::vec3 position;
    float scale;
    glm::vec3 rotation;
    glm::vec3 color;
};

auto create_boxes(const uint count) -> std::vector<Box>;
auto draw_Box(const Box& box, Shader& shader) -> void;

/**
 * @brief Starting point, function called by jac::main in main.hpp, contains the program loop
 * 
 * @param arg arguments passed to the program, processed by jac::main
 * @param env environment variables passed to the program, processed by jac::main
 * @retval int return status of the program
 */
auto run(jac::Arguments& /*arg*/, jac::Arguments& /*env*/) -> int
{
    initialize_glfw();
    unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window = create_window(1600, 1200, "OpenGL");

    if(!gladLoaderLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader(
        Resources::Shaders::basic_vertex,
        Resources::Shaders::basic_fragment
    );

    const std::vector<Box> boxes = create_boxes(8000);

    Model model = read_file("res/models/box.dat");

    VertexBuffer vb(model.vertices.data(), model.vertices.size() * sizeof(float));

    VertexArray va;
    va.AddBuffer(vb, model.layout);

    //### Loading texture
    Texture texture(Resources::Textures::container);
    Texture texture2(Resources::Textures::face);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //###

    struct State 
    {
        float mix = 0.2f;
        float cameraSpeed = 2.0f;

        Renderer::Camera camera{};
    };

    State state;
    using Input = Input<State>;

    Input input{window.get(), state};

    auto move = [](const glm::vec3& movement){
        return [movement](State& state, const float delta) {
            state.camera.move(movement * delta * state.cameraSpeed);
        };
    };

    auto roll = [](const float amount){
        return [amount](State& state, const float delta) {
            state.camera.roll(amount * delta * state.cameraSpeed);
        };
    };

    auto changeSpeed = [](const float speed){
        return [speed](State& state, const float) {
            state.cameraSpeed = speed;
        };
    };

    auto changeMix = [](const float mix){
        return [mix](State& state, const float delta) {
            state.mix = std::clamp(state.mix + mix * delta, 0.f, 1.f);
        };
    };

    auto toggleWireframeMode = [](const bool wireframe){
        return [wireframe](State&, const float) {
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        };
    };

    auto close = [](State&, const float) {
        glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
    };

    std::map<int, Input::KeyHandler> keyHandlers
    {
        {GLFW_KEY_W, { .held = move(glm::vec3{0.f, 0.f, 1.f})}},
        {GLFW_KEY_S, { .held = move(glm::vec3{0.f, 0.f, -1.f})}},
        {GLFW_KEY_A, { .held = move(glm::vec3{-1.f, 0.f, 0.f})}},
        {GLFW_KEY_D, { .held = move(glm::vec3{1.f, 0.f, 0.f})}},

        {GLFW_KEY_Q, { .held = roll(-10.f)}},
        {GLFW_KEY_E, { .held = roll(10.f)}},

        {GLFW_KEY_UP, { .held = changeMix(1.f)}},
        {GLFW_KEY_DOWN, { .held = changeMix(-1.f)}},

        {GLFW_KEY_LEFT_SHIFT, {
            .pressed = changeSpeed(10.f),
            .released = changeSpeed(2.f)}},

        {GLFW_KEY_TAB, {
            .pressed = toggleWireframeMode(true),
            .released = toggleWireframeMode(false)}},

        {GLFW_KEY_ESCAPE, { .pressed = close }}
    };

    for (const auto& [key, handler] : keyHandlers)
        input.setKeyHandler(key, handler);

    input.setMouseHandler([](State& state, const glm::vec2 delta, const glm::vec2 /*position*/) {
        glm::vec3 m_sensitivity{0.1f, 0.1f, 0.1f};
        glm::bvec3 m_invese{false, false, false};

        const glm::vec2 rotation =
            delta * glm::vec2{m_sensitivity.x, m_sensitivity.y};

        state.camera.yaw(rotation.x * (m_invese.x ? -1.f : 1.f));
        state.camera.pitch(rotation.y * (m_invese.y ? -1.f : 1.f));
    });

    input.setMouseScrollHandler([](State& state, const float delta) {
        state.camera.changeFov(-delta);
    });
    
    double time{};
    while(!glfwWindowShouldClose(window.get()))
    {
        time = glfwGetTime();

        input.update();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Bind();
        shader.SetUniform("uMix", state.mix);
        texture.Bind(0);
        shader.SetUniform("uTexture_0", 0);
        texture2.Bind(1);
        shader.SetUniform("uTexture_1", 1);

        va.Bind();

        shader.SetUniformM("uView", state.camera.getView());
        shader.SetUniformM("uProjection", state.camera.getProjection());

        for (const auto& box : boxes)
        {
            draw_Box(box, shader);
        }

        glfwSwapBuffers(window.get());

        constexpr double targetFPS = 60.0;
        while(glfwGetTime() - time < 1.0 / targetFPS);

        const uint fps = std::round(1.0 / (glfwGetTime() - time));
        const auto pos = state.camera.getPosition();
        std::cout << 
            '\r' << std::string(80, ' ') <<
            '\r' << std::format("FPS: {}, XYZ: {} {} {}", fps, pos.x, pos.y, pos.z) <<
            std::flush;
    }

    glfwTerminate();
    return 0;
}

auto initialize_glfw() -> void
{
    if(!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;

        int error_code = glfwGetError(nullptr);
        if (error_code == GLFW_PLATFORM_ERROR)
            std::cerr << "GLFW_PLATFORM_ERROR" << std::endl;
        else
            std::cerr << "Unknown error code" << std::endl;

        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__   
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
}

auto create_window(const uint width, const uint height, const std::string& title) -> unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>
{
    unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window { 
        glfwCreateWindow(
            width,
            height,
            title.c_str(),
            nullptr,
            nullptr),
        glfwDestroyWindow
    };

    if(window == nullptr)
    {
        std::cerr << "Failed to create GLFW window, error code: " << glfwGetError(nullptr) << std::endl;

        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window.get());
    glfwSetFramebufferSizeCallback(
        window.get(),
        [](GLFWwindow* /*window*/, int width, int height) { glViewport(0, 0, width, height);}
    );

    return window;
}

auto create_boxes(const uint count) -> std::vector<Box>
{
    std::vector<Box> boxes(count);

    srand(time(nullptr));
    auto randFloat = []() -> float {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    };

    for (auto& box : boxes)
    {
        box.position = glm::vec3(randFloat() * 100.f - 50.f, randFloat() * 100.f - 50.f, randFloat() * 100.f - 50.f);
        box.scale = randFloat() * 2.f;
        box.rotation = glm::vec3(randFloat() * 360.f, randFloat() * 360.f, randFloat() * 360.f);
        box.color = glm::vec3(randFloat(), randFloat(), randFloat());
    }

    auto& bigBox = boxes.back();
    bigBox.position = glm::vec3(1000.f, 1000.f, 1000.f);
    bigBox.scale = 1000.f;

    auto& skyBox = boxes.front();
    skyBox.position = glm::vec3(0.f, 0.f, 0.f);
    skyBox.scale = 5000.f;

    return boxes;
}

auto draw_Box(const Box& box, Shader& shader) -> void
{
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, box.position);
    model = glm::rotate(model, box.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, box.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, box.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(box.scale));

    shader.SetUniformM("uModel", model);
    shader.SetUniform("uColor", box.color.r, box.color.g, box.color.b, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}
