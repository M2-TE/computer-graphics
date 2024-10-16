FetchContent_Declare(vulkan-headers
    GIT_REPOSITORY "https://github.com/KhronosGroup/Vulkan-Headers.git"
    GIT_TAG "v1.3.290"
    GIT_SHALLOW ON
    GIT_SUBMODULES ""
    SOURCE_SUBDIR "disabled")
FetchContent_Declare(vulkan-hpp
    GIT_REPOSITORY "https://github.com/KhronosGroup/Vulkan-Hpp.git"
    GIT_TAG "v1.3.290"
    GIT_SHALLOW ON
    GIT_SUBMODULES ""
    SOURCE_SUBDIR "disabled")
FetchContent_MakeAvailable(vulkan-headers vulkan-hpp)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    "${vulkan-headers_SOURCE_DIR}/include"
	"${vulkan-hpp_SOURCE_DIR}")
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    "VULKAN_HPP_NO_SETTERS"
    "VULKAN_HPP_NO_TO_STRING"
    "VULKAN_HPP_NO_CONSTRUCTORS"
    "VULKAN_HPP_NO_SMART_HANDLE"
    "VULKAN_HPP_NO_SPACESHIP_OPERATOR"
    "VULKAN_HPP_TYPESAFE_CONVERSION=0"
    "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC")