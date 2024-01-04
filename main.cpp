#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <memory>
#include <iostream>

#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexBufferLayout.hpp"

#if !defined(OpenGL_VERSION_MAJOR) || !defined(OpenGL_VERSION_MINOR)
    #define OpenGL_VERSION_MAJOR 3
    #define OpenGL_VERSION_MINOR 3
#endif

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

namespace Resources {
    namespace Shaders {
        const std::filesystem::path basic_vertex = "res/shaders/basic.vert";
        const std::filesystem::path basic_fragment = "res/shaders/basic.frag";
    }
}

int main()
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
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window { glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr), glfwDestroyWindow };
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

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
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
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("res/textures/braun.png", &width, &height, &nrChannels, 0);

    if(data == nullptr)
    {
        std::cerr << "Failed to load texture" << std::endl;
        return -1;
    }

    if(nrChannels != 3 && nrChannels != 4)
    {
        std::cerr << "Invalid number of channels" << std::endl;
        return -1;
    }

    glTexImage2D(
        GL_TEXTURE_2D,                      // target (1D, 2D or 3D)
        0,                                  // mipmap level (0 = base level)
        nrChannels == 4? GL_RGBA: GL_RGB, // internal format (how OpenGL will store the texture)
        width,                              // width (of the texture)
        height,                             // height <-->
        0,                                  // border (legacy stuff, always 0)
        nrChannels == 4? GL_RGBA: GL_RGB, // format (how the data is stored in RAM)
        GL_UNSIGNED_BYTE,                   // type (type of the data)
        data                                // data (self-explanatory)
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    //###
    double time;
    while(!glfwWindowShouldClose(window.get()))
    {
        time = glfwGetTime();
        process_input(window.get());

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw triangle
        shader.Bind();
        shader.SetUniform("uOffset", cosf(time * M_PI) / 8.f, sinf(time * M_PI) / 8.f, 0.0f);

        // glBindTexture(GL_TEXTURE_2D, texture);

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