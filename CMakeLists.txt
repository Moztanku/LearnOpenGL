cmake_minimum_required(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wno-unused-parameter -Wno-unused-variable")

set(GLAD_DIR ${CMAKE_SOURCE_DIR}/GLAD)

add_executable(${PROJECT_NAME} main.cpp ${GLAD_DIR}/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE ${GLAD_DIR}/include)

set(OpenGLPackages
        OpenGL
        glfw3)

foreach(package ${OpenGLPackages})
    find_package(${package} REQUIRED)
    if(NOT ${package}_FOUND)
        message(FATAL_ERROR "${package} not found")
    endif()
endforeach()

target_link_libraries(${PROJECT_NAME} glfw GL)

target_compile_definitions(${PROJECT_NAME} PRIVATE OpenGL_VERSION_MAJOR=4 OpenGL_VERSION_MINOR=6)