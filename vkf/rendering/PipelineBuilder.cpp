////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PipelineBuilder.h
/// \brief This file implements the PipelineBuilder class which is used for building Vulkan pipelines.
///
/// The PipelineBuilder class is part of the vkf::rendering namespace. It provides functionality to set various states
/// of the pipeline and build a new Pipeline using these states.
///
/// \author Joshua Lowe
/// \date 12/15/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PipelineBuilder.h"
#include "../core/Device.h"
#include "../core/Shader.h"

namespace vkf::rendering
{

PipelineBuilder::~PipelineBuilder() = default;

PipelineBuilder &PipelineBuilder::setShaderStageCreateInfos(const core::Device &device, core::Shader &shader)
{
    this->pipelineShader = std::make_shared<core::Shader>(std::move(shader));
    state.shaderStageCreateInfos = this->pipelineShader->createShaderStages(device);
    return *this;
}

PipelineBuilder &PipelineBuilder::setVertexInputCreateInfo(
    const vk::PipelineVertexInputStateCreateInfo &info, vk::VertexInputBindingDescription &bindingDescription,
    std::vector<vk::VertexInputAttributeDescription> &attributeDescriptions)
{
    this->vertexInputBindingDescription = std::move(bindingDescription);
    this->vertexInputAttributeDescriptions = std::move(attributeDescriptions);
    state.vertexInputCreateInfo = info;
    state.vertexInputCreateInfo.pVertexBindingDescriptions = &this->vertexInputBindingDescription;
    state.vertexInputCreateInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(this->vertexInputAttributeDescriptions.size()),
    state.vertexInputCreateInfo.pVertexAttributeDescriptions = this->vertexInputAttributeDescriptions.data();
    return *this;
}

PipelineBuilder &PipelineBuilder::setInputAssemblyCreateInfo(const vk::PipelineInputAssemblyStateCreateInfo &info)
{
    state.inputAssemblyCreateInfo = info;
    return *this;
}

PipelineBuilder &PipelineBuilder::setPipelineViewportStateCreateInfo(const vk::PipelineViewportStateCreateInfo &info)
{
    state.pipelineViewportStateCreateInfo = info;
    return *this;
}

PipelineBuilder &PipelineBuilder::setRasterizerCreateInfo(const vk::PipelineRasterizationStateCreateInfo &info)
{
    state.rasterizerCreateInfo = info;
    return *this;
}

PipelineBuilder &PipelineBuilder::setMultisamplingCreateInfo(const vk::PipelineMultisampleStateCreateInfo &info)
{
    state.multisamplingCreateInfo = info;
    return *this;
}

PipelineBuilder &PipelineBuilder::setDepthStencilCreateInfo(const vk::PipelineDepthStencilStateCreateInfo &info)
{
    state.depthStencilCreateInfo = info;
    return *this;
}

PipelineBuilder &PipelineBuilder::setColorBlendingCreateInfo(const vk::PipelineColorBlendStateCreateInfo &info,
                                                             vk::PipelineColorBlendAttachmentState &attachment)
{
    this->colorBlendAttachment = std::move(attachment);
    state.colorBlendingCreateInfo = info;
    state.colorBlendingCreateInfo.pAttachments = &this->colorBlendAttachment;
    return *this;
}

PipelineBuilder &PipelineBuilder::setDynamicStateCreateInfo(const vk::PipelineDynamicStateCreateInfo &info,
                                                            std::vector<vk::DynamicState> &states)
{
    this->dynamicStates = std::move(states);
    state.dynamicStateCreateInfo = info;
    state.dynamicStateCreateInfo.pDynamicStates = this->dynamicStates.data();
    return *this;
}

PipelineBuilder &PipelineBuilder::setPipelineLayout(const vk::PipelineLayout &layout)
{
    state.pipelineLayout = layout;
    return *this;
}

PipelineBuilder &PipelineBuilder::setRenderPass(const vk::RenderPass &pass)
{
    state.renderPass = pass;
    return *this;
}

core::Pipeline PipelineBuilder::build(const core::Device &device)
{
    return {device, state};
}
} // namespace vkf::rendering