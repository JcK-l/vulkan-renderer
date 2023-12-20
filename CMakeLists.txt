cmake_minimum_required(VERSION 3.26)
project(VulkanRenderer CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)

#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG ON) # Enable LTGC for debug builds
#set(CMAKE_LINK_DEPENDS_DEBUG_MODE ON) # Enable incremental linking for debug builds
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g1") # Reduce the amount of debug information

add_subdirectory(third_party)
add_subdirectory(vkf)
add_subdirectory(app)