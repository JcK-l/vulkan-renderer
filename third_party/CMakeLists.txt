cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)

add_library(vkf_dependencies INTERFACE)

set(PROJ_DB_PATH "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/share/proj/proj.db")
set(ENV{PROJ_LIB} ${PROJ_DB_PATH})

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(GDAL CONFIG REQUIRED)
find_package(PROJ CONFIG REQUIRED)


add_library(ImGuizmo STATIC)
target_sources(ImGuizmo PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/ImGuizmo.cpp"
)
target_include_directories(ImGuizmo PRIVATE
        ImGuizmo/
        ${imgui_INCLUDE_DIRS}
)
target_link_libraries(ImGuizmo PRIVATE imgui::imgui)

target_link_libraries(vkf_dependencies
        INTERFACE
        Vulkan::Vulkan
        GPUOpen::VulkanMemoryAllocator
        unofficial::shaderc::shaderc
        glm::glm
        spdlog::spdlog
        glfw
        imgui::imgui
        ImGuizmo
        EnTT::EnTT
        GDAL::GDAL
        PROJ::proj
)


target_include_directories(vkf_dependencies
        INTERFACE
        ${Stb_INCLUDE_DIR}
        ImGuizmo/
        ${imgui_INCLUDE_DIRS}
)