cmake_minimum_required(VERSION 3.26)
project(vkf CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
        PUBLIC
        core/Device.cpp
        core/Instance.cpp
        core/PhysicalDevice.cpp
        core/Queue.cpp
        core/Swapchain.cpp
        platform/Application.cpp
        platform/Window.cpp
        common/Input.cpp
        common/Utility.cpp
)


target_link_libraries(${PROJECT_NAME} PUBLIC glfw glm spdlog vma)

target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_CONSTRUCTORS
        GLFW_INCLUDE_VULKAN)


target_precompile_headers(${PROJECT_NAME} PUBLIC pch.h)