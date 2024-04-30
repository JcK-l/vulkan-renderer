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

PipelineBuilder::PipelineBuilder(const PipelineBuilder &other)
    : state(other.state), pipelineShader(other.pipelineShader), colorBlendAttachment(other.colorBlendAttachment),
      dynamicStates(other.dynamicStates), vertexInputBindingDescription(other.vertexInputBindingDescription),
      vertexInputAttributeDescriptions(other.vertexInputAttributeDescriptions)
{
}

PipelineBuilder::PipelineBuilder(PipelineBuilder &&other) noexcept
    : state(std::move(other.state)), pipelineShader(std::move(other.pipelineShader)),
      colorBlendAttachment(std::move(other.colorBlendAttachment)), dynamicStates(std::move(other.dynamicStates)),
      vertexInputBindingDescription(std::move(other.vertexInputBindingDescription)),
      vertexInputAttributeDescriptions(std::move(other.vertexInputAttributeDescriptions))
{
}

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
    this->vertexInputBindingDescription = std::make_shared<vk::VertexInputBindingDescription>(bindingDescription);
    this->vertexInputAttributeDescriptions =
        std::make_shared<std::vector<vk::VertexInputAttributeDescription>>(std::move(attributeDescriptions));

    state.vertexInputCreateInfo = info;
    state.vertexInputCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescription.get();
    state.vertexInputCreateInfo.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(this->vertexInputAttributeDescriptions->size()),
    state.vertexInputCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions->data();
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
    this->colorBlendAttachment = std::make_shared<vk::PipelineColorBlendAttachmentState>(attachment);
    state.colorBlendingCreateInfo = info;
    state.colorBlendingCreateInfo.pAttachments = colorBlendAttachment.get();
    return *this;
}

PipelineBuilder &PipelineBuilder::setDynamicStateCreateInfo(const vk::PipelineDynamicStateCreateInfo &info,
                                                            std::vector<vk::DynamicState> &states)
{
    this->dynamicStates = std::make_shared<std::vector<vk::DynamicState>>(std::move(states));
    state.dynamicStateCreateInfo = info;
    state.dynamicStateCreateInfo.pDynamicStates = dynamicStates->data();
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