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

#include <memory>
#include <iostream>
#include <format>

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
    #define OpenGL_VERSION_MAJOR 3
    #define OpenGL_VERSION_MINOR 3
#endif

using Renderer::Camera;
using Renderer::GPU::Shader;
using Renderer::GPU::Texture;
using Renderer::GPU::VertexArray;
using Renderer::GPU::VertexBuffer;
using Renderer::GPU::VertexBufferLayout;

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

namespace Resources {
    namespace Shaders {
        const std::filesystem::path basic_vertex = "res/shaders/basic.vert";
        const std::filesystem::path basic_fragment = "res/shaders/basic.frag";
    }

    namespace Textures {
        const std::filesystem::path container = "res/textures/container.png";
        const std::filesystem::path face = "res/textures/grandfather-face.png";
    }
}

struct Box {
    glm::vec3 position;
    float scale;
    glm::vec3 rotation;
    glm::vec3 color;
};

void draw_Box(const Box& box, Shader& shader)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, box.position);
    model = glm::rotate(model, box.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, box.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, box.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(box.scale));

    shader.SetUniformM("uModel", model);
    shader.SetUniform("uColor", box.color.r, box.color.g, box.color.b, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

float randFloat()
{
    static bool initialized = false;
    if (!initialized)
    {
        srand(time(nullptr));
        initialized = true;
    }
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

/**
 * @brief Starting point, function called by jac::main in main.hpp, contains the program loop
 * 
 * @param arg arguments passed to the program, processed by jac::main
 * @param env environment variables passed to the program, processed by jac::main
 * @retval int return status of the program
 */
int run(jac::Arguments& arg, jac::Arguments& env)
{
    // Initialize GLFW
    if(!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;

        int error_code = glfwGetError(nullptr);
        if (error_code == GLFW_PLATFORM_ERROR)
            std::cerr << "GLFW_PLATFORM_ERROR" << std::endl;
        else
            std::cerr << "Unknown error code" << std::endl;

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__   
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create window
    unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window { glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr), glfwDestroyWindow };
    if(window == nullptr)
    {
        int error_code = glfwGetError(nullptr);
        std::cerr << "Failed to create GLFW window, " << std::endl;

        switch (error_code){
            case GLFW_NOT_INITIALIZED:
                std::cerr << "GLFW_NOT_INITIALIZED" << std::endl;
                break;
            case GLFW_INVALID_ENUM:
                std::cerr << "GLFW_INVALID_ENUM" << std::endl;
                break;
            case GLFW_INVALID_VALUE:
                std::cerr << "GLFW_INVALID_VALUE" << std::endl;
                break;
            case GLFW_API_UNAVAILABLE:
                std::cerr << "GLFW_API_UNAVAILABLE" << std::endl;
                break;
            case GLFW_VERSION_UNAVAILABLE:
                std::cerr << "GLFW_VERSION_UNAVAILABLE" << std::endl;
                break;
            case GLFW_FORMAT_UNAVAILABLE:
                std::cerr << "GLFW_FORMAT_UNAVAILABLE" << std::endl;
                break;
            case GLFW_PLATFORM_ERROR:
                std::cerr << "GLFW_PLATFORM_ERROR" << std::endl;
                break;
            default:
                std::cerr << "Unknown error code" << std::endl;
                break;
        }

        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window.get());

    if(!gladLoaderLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window.get(), framebuffer_size_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader(
        Resources::Shaders::basic_vertex,
        Resources::Shaders::basic_fragment
    );

    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

    std::vector<Box> boxes(5000, Box{});

    for (idx i = 0; i < boxes.size(); i++)
    {
        boxes[i].position = glm::vec3(randFloat() * 100.f - 50.f, randFloat() * 100.f - 50.f, randFloat() * 100.f - 50.f);
        boxes[i].scale = randFloat() * 2.f;
        boxes[i].rotation = glm::vec3(randFloat() * 360.f, randFloat() * 360.f, randFloat() * 360.f);
        boxes[i].color = glm::vec3(randFloat(), randFloat(), randFloat());
    }

    auto& bigBox = boxes.back();
    bigBox.position = glm::vec3(1000.f, 1000.f, 1000.f);
    bigBox.scale = 1000.f;

    auto& skyBox = boxes.front();
    skyBox.position = glm::vec3(0.f, 0.f, 0.f);
    skyBox.scale = 5000.f;

    VertexBuffer vb(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(3);  // position
    // layout.Push<float>(3);  // color
    layout.Push<float>(2);  // texture coordinates

    VertexArray va;
    va.AddBuffer(vb, layout);

    // Loading texture

    Texture texture(Resources::Textures::container);
    Texture texture2(Resources::Textures::face);
    

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //###
    double time;

    struct State 
    {
        float mix = 0.2f;
        float cameraSpeed = 2.0f;

        Renderer::Camera camera{};
    };

    State state;

    using Input = Input<State>;
    Input input{window.get(), state};

    input.setKeyHandler(GLFW_KEY_ESCAPE, {
        .pressed = [](State&, const float) {
            glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
        }
    });

    auto move = [](const glm::vec3& movement){
        return [movement](State& state, const float delta) {
            state.camera.move(movement * delta * state.cameraSpeed);
        };
    };

    input.setKeyHandler(GLFW_KEY_W, {
        .held = move(glm::vec3{0.f, 0.f, 1.f})
    });

    input.setKeyHandler(GLFW_KEY_S, {
        .held = move(glm::vec3{0.f, 0.f, -1.f})
    });

    input.setKeyHandler(GLFW_KEY_A, {
        .held = move(glm::vec3{-1.f, 0.f, 0.f})
    });

    input.setKeyHandler(GLFW_KEY_D, {
        .held = move(glm::vec3{1.f, 0.f, 0.f})
    });

    auto roll = [](const float amount){
        return [amount](State& state, const float delta) {
            state.camera.roll(amount * delta * state.cameraSpeed);
        };
    };

    input.setKeyHandler(GLFW_KEY_Q, {
        .held = roll(-10.f)
    });

    input.setKeyHandler(GLFW_KEY_E, {
        .held = roll(10.f)
    });

    input.setKeyHandler(GLFW_KEY_LEFT_SHIFT, {
        .pressed = [](State& state, const float) {
            state.cameraSpeed = 10.f;
        },
        .released = [](State& state, const float) {
            state.cameraSpeed = 2.f;
        }
    });

    input.setKeyHandler(GLFW_KEY_TAB, {
        .pressed = [](State&, const float) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        },
        .released = [](State&, const float) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    });

    input.setKeyHandler(GLFW_KEY_UP, {
        .held = [](State& state, const float delta) {
            state.mix = std::clamp(state.mix + delta, 0.f, 1.f);
        }
    });

    input.setKeyHandler(GLFW_KEY_DOWN, {
        .held = [](State& state, const float delta) {
            state.mix = std::clamp(state.mix - delta, 0.f, 1.f);
        }
    });

    input.setMouseHandler([](State& state, const glm::vec2 delta, const glm::vec2 position) {
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
    
    while(!glfwWindowShouldClose(window.get()))
    {
        time = glfwGetTime();

        input.update();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw triangle
        shader.Bind();
        shader.SetUniform("uMix", state.mix);
        texture.Bind(0);
        shader.SetUniform("uTexture_0", 0);
        texture2.Bind(1);
        shader.SetUniform("uTexture_1", 1);

        va.Bind();

        float x = sinf(time * M_PI);
        float z = cosf(time * M_PI);

        shader.SetUniformM("uView", state.camera.getView());

        shader.SetUniformM("uProjection", state.camera.getProjection());

        for (const auto& box : boxes)
        {
            draw_Box(box, shader);
        }

        glfwSwapBuffers(window.get());
        while(glfwGetTime() - time < 1.0 / 60.0);

        const uint fps = 1.0 / (glfwGetTime() - time);
        const auto pos = state.camera.getPosition();
        std::cout << 
            '\r' << std::string(80, ' ') <<
            '\r' << std::format("FPS: {}, XYZ: {} {} {}", fps, pos.x, pos.y, pos.z) <<
            std::flush;
    }

    glfwTerminate();
    return 0;
}
