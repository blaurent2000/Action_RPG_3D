
cmake_minimum_required(VERSION 3.20)

project(OpenGL3D)
find_package(OpenGL REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glm)

# GLAD
add_library(glad STATIC
    external/glad/src/gl.c
)

target_include_directories(glad PUBLIC
    external/glad/include
)

# Executable
add_executable(OpenGL3D
    src/main.cpp
)

target_link_libraries(OpenGL3D PRIVATE
    glfw
    glad
    glm::glm
    OpenGL::GL
)
