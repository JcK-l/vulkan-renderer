cmake_minimum_required(VERSION 3.26)
project(VulkanRenderer)

set(CMAKE_CXX_STANDARD 20)

add_executable(VulkanRenderer src/main.cpp
        src/application/Application.cpp
        src/application/Application.h
        src/application/Window.cpp
        src/application/Window.h
        src/common/utils.h
        src/core/Instance.cpp
        src/core/Instance.h
        src/common/event.h
        src/common/input.h
        src/common/logging.h
        src/common/utils.cpp
        src/common/logging.cpp
        src/common/input.cpp
)

target_precompile_headers(VulkanRenderer PUBLIC src/pch.h)



find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

set(glm_DIR ${CMAKE_SOURCE_DIR}/lib/glm/cmake/glm) # if necessary
find_package(glm REQUIRED)
target_link_libraries(${PROJECT_NAME} glm::glm)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glfw-3.3.8)
include_directories(${CMAKE_SOURCE_DIR}/lib/glfw-3.3.8/include)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})

set(spdlog_DIR ${CMAKE_SOURCE_DIR}/lib/spdlog/build)
find_package(spdlog REQUIRED)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)

