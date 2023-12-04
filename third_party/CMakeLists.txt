cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)

add_library(vkf_dependencies INTERFACE)

# khronos utilities
set(SPIRV_WERROR OFF)
##set(ALLOW_EXTERNAL_SPIRV_TOOLS ON)
set(ENABLE_OPT 0)
add_subdirectory(glslang)
#add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)

# VulkanMemoryAllocator
find_package(Vulkan REQUIRED)
target_include_directories(vkf_dependencies INTERFACE VulkanMemoryAllocator/include)
target_link_libraries(vkf_dependencies INTERFACE Vulkan::Vulkan)

# glm
add_subdirectory(glm)
target_link_libraries(vkf_dependencies INTERFACE glm)
target_include_directories(vkf_dependencies INTERFACE glm)

# spdlog
add_subdirectory(spdlog)
target_link_libraries(vkf_dependencies INTERFACE spdlog)

# GLFW
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)

add_subdirectory(glfw)
target_link_libraries(vkf_dependencies INTERFACE glfw)
target_include_directories(vkf_dependencies INTERFACE glfw/include/)

# imgui
add_library(ImGui STATIC)
target_sources(ImGui PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.cpp"
)
target_include_directories(ImGui PUBLIC imgui/backends imgui/)
target_link_libraries(ImGui PRIVATE glfw Vulkan::Vulkan)
target_link_libraries(vkf_dependencies INTERFACE ImGui)
target_include_directories(vkf_dependencies INTERFACE imgui/backends imgui/ imgui/misc/cpp/)

# EnTT
add_subdirectory(entt)
target_link_libraries(vkf_dependencies INTERFACE EnTT)
target_include_directories(vkf_dependencies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/entt/single_include/)

# shaderc
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_ENABLE_TESTS OFF)

add_subdirectory(shaderc)
target_link_libraries(vkf_dependencies INTERFACE shaderc)