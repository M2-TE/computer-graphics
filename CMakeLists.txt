cmake_minimum_required(VERSION 3.24)
project(opengl-renderer)

# Global settings
set(PREFER_EMBED_MODELS OFF CACHE BOOL "Prefer embedding models into executable" FORCE)
set(BUILD_SHARED_LIBS   ON  CACHE BOOL "Build shared instead of static libraries" FORCE)
# Isolated settings
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(SDL3Settings)
include(AssimpSettings)
include(GLbindingSettings)
include(MiscSettings)

include(FetchContent)
FetchContent_Declare(sdl3 GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git" GIT_TAG "main")
FetchContent_Declare(assimp GIT_REPOSITORY "https://github.com/assimp/assimp.git" GIT_TAG "master")
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm.git" GIT_TAG "master")
FetchContent_Declare(stb GIT_REPOSITORY "https://github.com/M2-TE/stb_image.git" GIT_TAG "master")
FetchContent_Declare(cmrc GIT_REPOSITORY "https://github.com/vector-of-bool/cmrc.git" GIT_TAG "master")
FetchContent_Declare(glbinding GIT_REPOSITORY "https://github.com/cginternals/glbinding.git" GIT_TAG "master")
FetchContent_MakeAvailable(sdl3 assimp glm stb cmrc glbinding)
find_package(OpenGL REQUIRED COMPONENTS)

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
target_link_libraries(${PROJECT_NAME} OpenGL::GL) # OpenGL headers
target_link_libraries(${PROJECT_NAME} glbinding::glbinding) # OpenGL function loader
target_link_libraries(${PROJECT_NAME} assimp stb-image) # model/image loading
target_link_libraries(${PROJECT_NAME} shaders images) # CMRC embedded files

# only embed models with CMRC if desired
if (${PREFER_EMBED_MODELS})
    file(GLOB_RECURSE model-files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS "models/*")
    cmrc_add_resource_library(models "${model-files}") # embed models into library "models"
    target_link_libraries(${PROJECT_NAME} models)
    target_compile_definitions(${PROJECT_NAME} PRIVATE EMBEDDED_MODELS=1)
endif()