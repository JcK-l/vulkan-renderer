cmake_minimum_required(VERSION 3.26)
project(VulkanRenderer CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(third_party)
add_subdirectory(vkf)
add_subdirectory(app)