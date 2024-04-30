cmake_minimum_required(VERSION 3.26)

set(CMAKE_CXX_STANDARD 20)

add_library(vkf STATIC)

# Core
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
)

# Platform
target_sources(vkf
        PUBLIC
        platform/Application.cpp
        platform/Window.cpp
        platform/Gui.cpp
)

# Common
target_sources(vkf
        PUBLIC
        common/Input.cpp
        common/Utility.cpp
        common/UUID.cpp
        common/GeometryHandling.cpp
)

# Rendering
target_sources(vkf
        PUBLIC
        rendering/RenderManager.cpp
        rendering/FrameData.cpp
        rendering/Renderer.cpp
        rendering/RenderSource.cpp
        rendering/RenderSubstage.cpp
        rendering/ForwardSubstage.cpp
        rendering/GuiSubstage.cpp
        rendering/PipelineBuilder.cpp
        rendering/BindlessManager.cpp
)

# Scene
target_sources(vkf
        PUBLIC
        scene/Scene.cpp
        scene/Camera.cpp
        scene/Entity.cpp
        scene/prefabs/PrefabFactory.cpp
        scene/prefabs/Prefab.cpp
        scene/prefabs/Cube.cpp
        scene/prefabs/Texture2D.cpp
        scene/prefabs/BasemapActor.cpp
        scene/prefabs/GraticuleActor.cpp
        scene/prefabs/PrefabTypeManager.cpp
        scene/prefabs/PoleActor.cpp
        scene/components/ColorComponent.cpp
        scene/components/TransformComponent.cpp
        scene/components/MaterialComponent.cpp
        scene/components/MeshComponent.cpp
        scene/components/TextureComponent.cpp
        scene/components/TagComponent.cpp
        scene/components/RelationComponent.cpp
        scene/components/GeotiffComponent.cpp
        scene/components/GraticuleComponent.cpp
        scene/components/ProjectionComponent.cpp
        scene/components/BoundingBoxComponent.cpp
        scene/components/PoleComponent.cpp
)


target_link_libraries(vkf PUBLIC vkf_dependencies)

target_compile_definitions(vkf PUBLIC VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_CONSTRUCTORS
        GLFW_INCLUDE_VULKAN GLM_FORCE_RADIANS -DPROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}" -DPROJECT_BUILD_DIR="${CMAKE_BINARY_DIR}")
#GLM_FORCE_LEFT_HANDED GLM_FORCE_DEPTH_ZERO_TO_ONE

target_precompile_headers(vkf PUBLIC pch.h)