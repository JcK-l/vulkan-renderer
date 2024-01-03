////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PrefabFactory.cpp
/// \brief This file implements the PrefabFactory class, which is used to create prefabricated entities in the
/// vkf::scene namespace.
///
/// The PrefabFactory class is part of the vkf::scene namespace. It provides an interface for creating prefabricated
/// entities, such as Cubes and Triangles. These entities are used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrefabFactory.h"
#include "../../core/Device.h"
#include "../../core/RenderPass.h"
#include "../../core/Shader.h"
#include "../../rendering/BindlessManager.h"
#include "../../rendering/PipelineBuilder.h"

namespace vkf::scene
{

PrefabFactory::PrefabFactory(const core::Device &device, rendering::BindlessManager &bindlessManager,
                             const core::RenderPass &renderPass, Camera *camera)
    : device{device}, bindlessManager{bindlessManager}, renderPass{renderPass}, camera{camera}
{
    createPipelines();
}

PrefabFactory::~PrefabFactory() = default;

void PrefabFactory::createPipelines()
{
    rendering::PipelineBuilder pipelineBuilder;

    core::Shader cubeShader{"../../shaders/cube.glsl"};
    pipelineBuilder.setShaderStageCreateInfos(cubeShader.createShaderStages(device));

    //    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{
    //        .vertexBindingDescriptionCount = 1,
    //        .pVertexBindingDescriptions = &Cube::getBindingDescription(),
    //        .vertexAttributeDescriptionCount = static_cast<uint32_t>(Cube::getAttributeDescriptions().size()),
    //        .pVertexAttributeDescriptions = Cube::getAttributeDescriptions().data()
    //    };

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

    pipelines.emplace(PrefabType::Cube, std::make_unique<core::Pipeline>(pipelineBuilder.build(device)));

    core::Shader triangleShader{"../../shaders/triangle.glsl"};
    pipelineBuilder.setShaderStageCreateInfos(triangleShader.createShaderStages(device));

    pipelines.emplace(PrefabType::Triangle, std::make_unique<core::Pipeline>(pipelineBuilder.build(device)));
}

std::unique_ptr<Entity> PrefabFactory::createCustom(entt::registry &registry, std::string tag)
{
    auto entity = std::make_unique<Entity>(registry, bindlessManager);
    entity->create(std::move(tag));
    return entity;
}

} // namespace vkf::scene