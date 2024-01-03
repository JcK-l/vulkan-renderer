////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Pipeline.cpp
/// \brief This file implements the Pipeline class which is used for managing Vulkan pipelines.
///
/// The Pipeline class is part of the vkf::core namespace. It provides functionality to get the handle to the pipeline.
///
/// \author Joshua Lowe
/// \date 12/3/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Pipeline.h"
#include "Device.h"
#include "RenderPass.h"
#include "Shader.h"

namespace vkf::core
{

Pipeline::Pipeline(const Device &device, const PipelineState &state)
{
    auto pipelineCreateInfo = vk::GraphicsPipelineCreateInfo{.stageCount = 2,
                                                             .pStages = state.shaderStageCreateInfos.data(),
                                                             .pVertexInputState = &state.vertexInputCreateInfo,
                                                             .pInputAssemblyState = &state.inputAssemblyCreateInfo,
                                                             .pViewportState = &state.pipelineViewportStateCreateInfo,
                                                             .pRasterizationState = &state.rasterizerCreateInfo,
                                                             .pMultisampleState = &state.multisamplingCreateInfo,
                                                             .pDepthStencilState = &state.depthStencilCreateInfo,
                                                             .pColorBlendState = &state.colorBlendingCreateInfo,
                                                             .pDynamicState = &state.dynamicStateCreateInfo,
                                                             .layout = state.pipelineLayout,
                                                             .renderPass = state.renderPass};

    handle = vk::raii::Pipeline{device.getHandle(), nullptr, pipelineCreateInfo};
}

const vk::raii::Pipeline &Pipeline::getHandle() const
{
    return handle;
}

} // namespace vkf::core