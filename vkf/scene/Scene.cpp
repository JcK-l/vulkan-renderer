////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Scene.cpp
/// \brief This file defines the Scene class which is used for managing a scene.
///
/// The Scene class is part of the vkf::scene namespace. It provides an interface for interacting with a scene.
///
/// \author Joshua Lowe
/// \date 12/3/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Scene.h"
#include "../core/Buffer.h"
#include "../core/Pipeline.h"
#include "../core/RenderPass.h"
#include "../core/Shader.h"
#include "../rendering/BindlessManager.h"
#include "../rendering/PipelineBuilder.h"
#include "Camera.h"
#include "Entity.h"

namespace vkf::scene
{

Scene::Scene(const core::Device &device, rendering::BindlessManager &bindlessManager,
             const core::RenderPass &renderPass, Camera &camera)
    : device{device}, bindlessManager{bindlessManager}, renderPass{renderPass},
      sceneCamera{std::make_unique<Camera>(camera)}
{
}

Scene::~Scene() = default;

Entity Scene::createObject(const std::string &name, const glm::vec3 &position, const glm::vec3 &rotation,
                           const glm::vec3 &scale, const glm::vec4 &color, const std::string &shader)
{
    auto entity = createEntity(name);

    entity.addComponent<scene::TransformComponent>(position, rotation, scale);
    entity.addComponent<scene::ColorComponent>(color);

    struct alignas(16) Data
    {
        float data[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    } data;

    vk::BufferCreateInfo bufferCreateInfo{.size = sizeof(data), .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer buffer{device, bufferCreateInfo,
                        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    vk::BufferCreateInfo bufferModelCreateInfo{.size = 64, .usage = vk::BufferUsageFlagBits::eUniformBuffer};
    core::Buffer bufferModel{device, bufferModelCreateInfo,
                             VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    auto pipeline = createPipeline(shader);
    entity.addComponent<scene::MaterialComponent>(pipeline);
    auto &entityMaterialComponent = entity.getComponent<scene::MaterialComponent>();
    entityMaterialComponent.addBuffer("camera", sceneCamera->getHandle());

    auto entityBufferHandle = bindlessManager.storeBuffer(buffer, vk::BufferUsageFlagBits::eUniformBuffer);
    entityMaterialComponent.addBuffer("color", entityBufferHandle);

    auto entityBufferModelHandle = bindlessManager.storeBuffer(bufferModel, vk::BufferUsageFlagBits::eUniformBuffer);
    entityMaterialComponent.addBuffer("model", entityBufferModelHandle);

    return entity;
}

Entity Scene::createCube(const std::string &name, const glm::vec3 &position, const glm::vec3 &rotation,
                         const glm::vec3 &scale, const glm::vec4 &color)
{
    auto entity = createObject(name, position, rotation, scale, color, "../../shaders/cube.glsl");
    return entity;
}

core::Pipeline Scene::createPipeline(const std::string &shaderName)
{
    rendering::PipelineBuilder pipelineBuilder;

    core::Shader shader{shaderName};
    pipelineBuilder.setShaderStageCreateInfos(shader.createShaderStages(device));
    pipelineBuilder.setVertexInputCreateInfo(vk::PipelineVertexInputStateCreateInfo{});
    pipelineBuilder.setInputAssemblyCreateInfo(
        vk::PipelineInputAssemblyStateCreateInfo{.topology = vk::PrimitiveTopology::eTriangleList});
    pipelineBuilder.setPipelineViewportStateCreateInfo(vk::PipelineViewportStateCreateInfo{
        .viewportCount = 1, .pViewports = nullptr, .scissorCount = 1, .pScissors = nullptr});
    pipelineBuilder.setRasterizerCreateInfo(vk::PipelineRasterizationStateCreateInfo{
        .polygonMode = vk::PolygonMode::eFill, .frontFace = vk::FrontFace::eCounterClockwise, .lineWidth = 1.0f});
    pipelineBuilder.setMultisamplingCreateInfo(
        vk::PipelineMultisampleStateCreateInfo{.rasterizationSamples = vk::SampleCountFlagBits::e1});

    pipelineBuilder.setDepthStencilCreateInfo(
        vk::PipelineDepthStencilStateCreateInfo{.depthTestEnable = VK_TRUE,
                                                .depthWriteEnable = VK_TRUE,
                                                .depthCompareOp = vk::CompareOp::eLess,
                                                .depthBoundsTestEnable = VK_FALSE,
                                                .stencilTestEnable = VK_FALSE});

    auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState{
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};
    pipelineBuilder.setColorBlendingCreateInfo(
        vk::PipelineColorBlendStateCreateInfo{.attachmentCount = 1, .pAttachments = &colorBlendAttachment});

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    pipelineBuilder.setDynamicStateCreateInfo(vk::PipelineDynamicStateCreateInfo{
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data()});

    auto pipelineLayout = bindlessManager.getPipelineLayout();
    pipelineBuilder.setPipelineLayout(pipelineLayout);
    pipelineBuilder.setRenderPass(*renderPass.getHandle());

    return pipelineBuilder.build(device);
}

Entity Scene::createEntity(const std::string &name)
{
    Entity entity{registry};
    entity.create();
    entity.addComponent<TagComponent>(name);
    return entity;
}

entt::registry &Scene::getRegistry()
{
    return registry;
}

Camera *Scene::getCamera() const
{
    return sceneCamera.get();
}

} // namespace vkf::scene