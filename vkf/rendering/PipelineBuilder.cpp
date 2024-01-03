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

namespace vkf::rendering
{

PipelineBuilder &PipelineBuilder::setShaderStageCreateInfos(const std::vector<vk::PipelineShaderStageCreateInfo> &infos)
{
    state.shaderStageCreateInfos = infos;
    return *this;
}

PipelineBuilder &PipelineBuilder::setVertexInputCreateInfo(const vk::PipelineVertexInputStateCreateInfo &info)
{
    state.vertexInputCreateInfo = info;
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

PipelineBuilder &PipelineBuilder::setColorBlendingCreateInfo(const vk::PipelineColorBlendStateCreateInfo &info)
{
    state.colorBlendingCreateInfo = info;
    return *this;
}

PipelineBuilder &PipelineBuilder::setDynamicStateCreateInfo(const vk::PipelineDynamicStateCreateInfo &info)
{
    state.dynamicStateCreateInfo = info;
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