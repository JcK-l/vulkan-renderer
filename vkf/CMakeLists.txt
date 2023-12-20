cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)

add_library(vkf)

target_sources(vkf
        PUBLIC
        core/Device.cpp
        core/Instance.cpp
        core/PhysicalDevice.cpp
        core/Queue.cpp
        core/Swapchain.cpp
        core/Framebuffer.cpp
        core/Image.cpp
        core/Buffer.cpp
        core/RenderPass.cpp
        core/CommandPool.cpp
        core/Pipeline.cpp
        core/Shader.cpp
        platform/Application.cpp
        platform/Window.cpp
        platform/Gui.cpp
        common/Input.cpp
        common/Utility.cpp
        rendering/RenderManager.cpp
        rendering/FrameData.cpp
        rendering/Renderer.cpp
        rendering/RenderSource.cpp
        rendering/RenderSubstage.cpp
        rendering/ForwardSubstage.cpp
        rendering/GuiSubstage.cpp
        rendering/PipelineBuilder.cpp
        rendering/BindlessManager.cpp
        scene/Scene.cpp
        scene/Camera.cpp
        scene/Entity.cpp
)


target_link_libraries(vkf PUBLIC vkf_dependencies)

target_compile_definitions(vkf PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_CONSTRUCTORS
        GLFW_INCLUDE_VULKAN GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_RADIANS GLM_FORCE_LEFT_HANDED)


target_precompile_headers(vkf PUBLIC pch.h)