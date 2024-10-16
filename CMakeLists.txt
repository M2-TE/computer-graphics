cmake_minimum_required(VERSION 3.24)
project(opengl-renderer LANGUAGES C CXX)

# some settings
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_SHARED_LIBS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

# create renderer executable
add_executable(${PROJECT_NAME} 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ext.cpp"
)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# fetch project dependencies
include(FetchContent)
set(FETCHCONTENT_QUIET OFF) # enable git output for FetchContent steps
set(FETCHCONTENT_UPDATES_DISCONNECTED ON) # speed up consecutive config runs
include(sdl)
include(vulkan)
include(fmt)