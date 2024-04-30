////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Prefab.cpp
/// \brief This file implements the abstract Prefab class which is used for managing prefabricated entities in a scene.
///
/// The Prefab class is part of the vkf::scene namespace.
/// It provides an abstract interface for creating and managing prefabricated entities.
///
/// \author Joshua Lowe
/// \date 1/6/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Prefab.h"
#include "../../core/RenderPass.h"
#include "../../rendering/BindlessManager.h"
#include "../../rendering/PipelineBuilder.h"

namespace vkf::scene
{

rendering::PipelineBuilder Prefab::getPipelineBuilder(const core::Device &device, const core::RenderPass &renderPass,
                                                      rendering::BindlessManager &bindlessManager)
{
    rendering::PipelineBuilder pipelineBuilder;

    pipelineBuilder.setInputAssemblyCreateInfo(
        vk::PipelineInputAssemblyStateCreateInfo{.topology = vk::PrimitiveTopology::eTriangleList});

    auto bindingDescription = vk::VertexInputBindingDescription{};
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {};
    auto vertexInfo = vk::PipelineVertexInputStateCreateInfo{};

    pipelineBuilder.setVertexInputCreateInfo(vertexInfo, bindingDescription, attributeDescriptions);
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
    pipelineBuilder.setColorBlendingCreateInfo(vk::PipelineColorBlendStateCreateInfo{.attachmentCount = 1},
                                               colorBlendAttachment);

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    pipelineBuilder.setDynamicStateCreateInfo(
        vk::PipelineDynamicStateCreateInfo{.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size())},
        dynamicStates);

    auto &pipelineLayout = bindlessManager.getPipelineLayout();
    pipelineBuilder.setPipelineLayout(pipelineLayout);
    pipelineBuilder.setRenderPass(*renderPass.getHandle());

    return pipelineBuilder;
}

entt::entity Prefab::getEntity()
{
    return entity.getHandle();
}

} // namespace vkf::scene