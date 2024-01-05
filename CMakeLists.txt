cmake_minimum_required(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
project(LearnOpenGL)

### Compiler setup
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

option(Strict "Strict" OFF)

if(Strict MATCHES OFF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-comment")
endif()

### Vendor setup (GLAD, STB, JacekLib)
set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

set(STB_DIR ${VENDOR_DIR}/stb)

set(GLAD_DIR ${VENDOR_DIR}/GLAD)
file(GLOB GLAD_SRC ${GLAD_DIR}/src/glad.c)


set(JacekLib_DIR ${VENDOR_DIR}/JacekLib)
add_subdirectory(${JacekLib_DIR})

### Source setup

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/**.cpp)
set(HEADERS ${CMAKE_SOURCE_DIR}/inc ${GLAD_DIR}/include ${STB_DIR}/inc ${JacekLib_DIR}/inc)

### OpenGL setup
set(OpenGLPackages
        OpenGL
        glfw3
        glm)

foreach(package ${OpenGLPackages})
    find_package(${package} REQUIRED)
    if(NOT ${package}_FOUND)
        message(FATAL_ERROR "${package} not found")
    endif()
endforeach()

### Project setup
add_executable(${PROJECT_NAME} main.cpp ${GLAD_SRC} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})
target_link_libraries(${PROJECT_NAME} glfw GL JacekLib)
target_compile_definitions(${PROJECT_NAME} PRIVATE OpenGL_VERSION_MAJOR=3 OpenGL_VERSION_MINOR=3)