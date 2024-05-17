cmake_minimum_required(VERSION 3.28)
project(LearnOpenGL)

### Compiler setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

option(Strict "Strict" OFF)

if(Strict MATCHES OFF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wno-sign-compare -Wno-comment")
endif()

### Vendor setup (GLAD, STB, JacekLib)
set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

## JacekLib
set(JacekLib_DIR ${VENDOR_DIR}/JacekLib)
set(JacekLib_URL "http://github.com/Moztanku/JacekLib/archive/main.zip")

if (NOT EXISTS ${JacekLib_DIR})
    file(DOWNLOAD ${JacekLib_URL} ${VENDOR_DIR}/JacekLib.zip SHOW_PROGRESS)
    file(ARCHIVE_EXTRACT
        INPUT ${VENDOR_DIR}/JacekLib.zip
        DESTINATION ${VENDOR_DIR})

    file(RENAME ${VENDOR_DIR}/JacekLib-main ${JacekLib_DIR})
    file(REMOVE ${VENDOR_DIR}/JacekLib.zip)
endif()

## STB_image
set(STB_DIR ${VENDOR_DIR}/stb)
set(STB_IMG_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")

if (NOT EXISTS ${STB_DIR}/inc/stb_image.h)
    file(DOWNLOAD ${STB_IMG_URL} ${STB_DIR}/inc/stb_image.h SHOW_PROGRESS)
endif()

## GLAD
set(GLAD_DIR ${VENDOR_DIR}/GLAD)
set(GLAD_URL "https://gitfront.io/r/Moztanku/8s4L1GHF3Tsc/Resources/raw/glad.zip")

if (NOT EXISTS ${GLAD_DIR})
    file(DOWNLOAD ${GLAD_URL} ${GLAD_DIR}/glad.zip SHOW_PROGRESS
        STATUS status)

    list(GET status 0 error_code)

    if (NOT ${error_code} EQUAL 0)
        file(REMOVE_RECURSE ${GLAD_DIR})
        message(FATAL_ERROR "Failed to download GLAD, generate it through: 'https://gen.glad.sh/#generator=c&api=gl%3D4.6&profile=gl%3Dcore%2Cgles1%3Dcommon&options=LOADER' and place it in ${GLAD_DIR}")
    endif()

    file(ARCHIVE_EXTRACT
        INPUT ${GLAD_DIR}/glad.zip
        DESTINATION ${GLAD_DIR})

    file(REMOVE ${GLAD_DIR}/glad.zip)
endif()

add_library(glad STATIC ${GLAD_DIR}/src/gl.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

add_subdirectory(${JacekLib_DIR})

### Source setup
file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/**.cpp)
set(HEADERS ${CMAKE_SOURCE_DIR}/inc ${STB_DIR}/inc)

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
add_executable(${PROJECT_NAME} ${SOURCES} ${GLAD_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})
target_link_libraries(${PROJECT_NAME} glfw GL JacekLib glad)
target_compile_definitions(${PROJECT_NAME} PRIVATE OpenGL_VERSION_MAJOR=4 OpenGL_VERSION_MINOR=6)

#### Custom targets
add_custom_target(cleanup
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${VENDOR_DIR}
    COMMENT "Removing generated & downloaded files"
)