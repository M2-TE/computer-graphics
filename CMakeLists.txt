cmake_minimum_required(VERSION 3.20)
project(opengl-renderer)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# SDL3
include(FetchContent)
FetchContent_Declare(SDL3 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG main)
FetchContent_MakeAvailable(SDL3)

# GLAD
FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git GIT_TAG glad2)
FetchContent_Populate(glad)
add_subdirectory("${glad_SOURCE_DIR}/cmake")
glad_add_library(GLAD SHARED API gl:core=4.6)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED COMPONENTS)

# create executable target
add_executable(${PROJECT_NAME} "src/main.cpp")

# link libraries to executable
target_link_libraries(${PROJECT_NAME} SDL3-shared)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} GLAD)