#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>

#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"

#include "jac/main.hpp"
#include "jac/type_defs.hpp"

#if !defined(OpenGL_VERSION_MAJOR) || !defined(OpenGL_VERSION_MINOR)
    #define OpenGL_VERSION_MAJOR 3
    #define OpenGL_VERSION_MINOR 3
#endif

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

struct State 
{
    float mix = 0.2f;
};

void process_input(GLFWwindow* window, State& state)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        state.mix += 0.01f;
        if (state.mix > 1.0f)
            state.mix = 1.0f;
    }
    else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        state.mix -= 0.01f;
        if (state.mix < 0.0f)
            state.mix = 0.0f;
    }
}

namespace Resources {
    namespace Shaders {
        const std::filesystem::path basic_vertex = "res/shaders/basic.vert";
        const std::filesystem::path basic_fragment = "res/shaders/basic.frag";
    }
}

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
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    VertexBuffer vb(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.Push<float>(3);  // position
    layout.Push<float>(3);  // color
    layout.Push<float>(2);  // texture coordinates
    
    IndexBuffer ib(indices, sizeof(indices) / sizeof(*indices));

    VertexArray va;
    va.AddBuffer(vb, layout);

    // Wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Loading texture

    Texture texture("res/textures/container.png");
    Texture texture2("res/textures/braun.png");
    

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //###
    double time;

    State state;
    while(!glfwWindowShouldClose(window.get()))
    {
        time = glfwGetTime();
        process_input(window.get(), state);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw triangle
        shader.Bind();
        shader.SetUniform("uOffset", cosf(time * M_PI) / 8.f, sinf(time * M_PI) / 8.f, 0.0f);
        shader.SetUniform("uMix", state.mix);

        texture.Bind(0);
        shader.SetUniform("uTexture_0", 0);

        texture2.Bind(1);
        shader.SetUniform("uTexture_1", 1);

        va.Bind();
        ib.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window.get());
        glfwPollEvents();
        while(glfwGetTime() - time < 1.0 / 60.0);
    }

    glfwTerminate();
    return 0;
}