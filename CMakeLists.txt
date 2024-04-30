cmake_minimum_required(VERSION 3.26)

set(ENV{VCPKG_FEATURE_FLAGS} "manifests")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")

# Print the value of CMAKE_TOOLCHAIN_FILE
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

project(VulkanRenderer CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(third_party)
add_subdirectory(vkf)
add_subdirectory(app)