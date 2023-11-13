cmake_minimum_required(VERSION 3.28)
project(vkf CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
        PUBLIC
        platform/vkf.platform.Application.cpp
        platform/vkf.platform.Window.cpp
        core/vkf.core.Device.cpp
        core/vkf.core.PhysicalDevice.cpp
        core/vkf.core.Instance.cpp
        core/vkf.core.Queue.cpp
)

target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET CXX_MODULES FILES
        platform/vkf.platform.Application.ixx
        platform/vkf.platform.Window.ixx
        core/vkf.core.Device.ixx
        core/vkf.core.PhysicalDevice.ixx
        core/vkf.core.Instance.ixx
        core/vkf.core.Queue.ixx
        common/vkf.common.log.ixx
        common/vkf.common.utils.ixx
        common/vkf.common.input.ixx
)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw glm spdlog vma)

target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_CONSTRUCTORS GLFW_INCLUDE_VULKAN)