# try finding system package first
find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
    # fetch and build it ourselves
    include(FetchContent)
    FetchContent_Declare(sdl
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-3.2.24"
        GIT_SHALLOW ON)
    FetchContent_MakeAvailable(sdl)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

# SDL_image is a satelite library of SDL for image loading
find_package(SDL3_image QUIET)
if(NOT SDL3_image_FOUND)
    # fetch and build it ourselves
    include(FetchContent)
    FetchContent_Declare(sdl_image
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL_image.git"
        GIT_TAG "release-3.2.4"
        GIT_SHALLOW ON)
    FetchContent_MakeAvailable(sdl_image)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)