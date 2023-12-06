cmake_minimum_required(VERSION 3.20)
project(opengl-renderer)

## Settings
# Global
set(PREFER_EMBED_MODELS OFF CACHE BOOL "Prefer embedding models into executable" FORCE)
set(PREFER_SHARED ON CACHE BOOL "Prefer shared libraries for faster build times" FORCE)
# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
# SDL3
set(SDL_SHARED ON CACHE BOOL "Build SDL as shared lib" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Build SDL as static lib" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "Build SDL tests" FORCE)
# ASSIMP
set(BUILD_SHARED_LIBS ${PREFER_SHARED} CACHE BOOL "Build ASSIMP as shared lib" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build ASSIMP tests" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Enable ASSIMP install" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "Build ASSIMP viewer" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Build all ASSIMP exporters" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "Build all ASSIMP importers" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "Enable .obj importer" FORCE) # specifically only enable .obj importer
# Misc
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(sdl3 GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git" GIT_TAG "main")
FetchContent_Declare(assimp GIT_REPOSITORY "https://github.com/assimp/assimp.git" GIT_TAG "master")
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG "master")
FetchContent_Declare(stb GIT_REPOSITORY "https://github.com/M2-TE/stb_image.git" GIT_TAG "master")
FetchContent_Declare(cmrc GIT_REPOSITORY "https://github.com/vector-of-bool/cmrc.git" GIT_TAG "master")
FetchContent_Declare(glad GIT_REPOSITORY "https://github.com/Dav1dde/glad.git" GIT_TAG "glad2" SOURCE_SUBDIR "cmake")
FetchContent_MakeAvailable(sdl3 assimp glm stb cmrc glad)
find_package(OpenGL REQUIRED COMPONENTS)

if (${PREFER_SHARED})
    glad_add_library(GLAD SHARED API gl:core=4.6) # SHARED
else()
    glad_add_library(GLAD STATIC API gl:core=4.6) # STATIC
endif()
file(GLOB_RECURSE shader-files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS "shaders/*")
cmrc_add_resource_library(shaders "${shader-files}") # embed shaders into library "shaders"
file(GLOB_RECURSE image-files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS "images/*")
cmrc_add_resource_library(images "${image-files}") # embed images into library "images"

# create executable target
file(GLOB_RECURSE source-files CONFIGURE_DEPENDS "src/*")
add_executable(${PROJECT_NAME} "${source-files}")
target_include_directories(${PROJECT_NAME} PRIVATE "include/")

# link libraries to executable
target_link_libraries(${PROJECT_NAME} glm) # OpenGL math library
target_link_libraries(${PROJECT_NAME} SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} OpenGL::GL GLAD) # OpenGL headers and function loader
target_link_libraries(${PROJECT_NAME} assimp stb-image) # model/image loading
target_link_libraries(${PROJECT_NAME} shaders images) # CMRC embedded files

# only embed models with CMRC if desired
if (${PREFER_EMBED_MODELS})
    file(GLOB_RECURSE model-files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS "models/*")
    cmrc_add_resource_library(models "${model-files}") # embed models into library "models"
    target_link_libraries(${PROJECT_NAME} models)
    target_compile_definitions({$PROJECT_NAME} EMBEDDED_MODELS=1)
endif()