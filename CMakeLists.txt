cmake_minimum_required(VERSION 3.20)
project(opengl-renderer)
include(FetchContent)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED COMPONENTS)

# SDL3
set(SDL_SHARED ON)
set(SDL_STATIC OFF)
set(SDL_TEST_LIBRARY OFF)
FetchContent_Declare(sdl3 GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git" GIT_TAG "main")
FetchContent_MakeAvailable(sdl3)

# GLAD
FetchContent_Declare(glad GIT_REPOSITORY "https://github.com/Dav1dde/glad.git" GIT_TAG "glad2")
FetchContent_Populate(glad)
add_subdirectory("${glad_SOURCE_DIR}/cmake")
glad_add_library(GLAD SHARED API gl:core=4.6)

# GLM
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG "master")
FetchContent_MakeAvailable(glm)

# CMRC
FetchContent_Declare(cmrc GIT_REPOSITORY "https://github.com/vector-of-bool/cmrc.git" GIT_TAG "master")
FetchContent_Populate(cmrc)
add_subdirectory("${cmrc_SOURCE_DIR}")
cmrc_add_resource_library(shaders)
cmrc_add_resources(shaders WHENCE "shaders" "shaders/default.vs")
cmrc_add_resources(shaders WHENCE "shaders" "shaders/default.fs")

# create executable target
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE "src/main.cpp")
target_include_directories(${PROJECT_NAME} PRIVATE "include/")

# link libraries to executable
target_link_libraries(${PROJECT_NAME} SDL3-shared)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} GLAD)
target_link_libraries(${PROJECT_NAME} glm)
target_link_libraries(${PROJECT_NAME} shaders)