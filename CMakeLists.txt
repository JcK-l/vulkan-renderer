cmake_minimum_required(VERSION 3.28)
project(VulkanRenderer CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third-party)
add_subdirectory(vkf)
add_subdirectory(app)