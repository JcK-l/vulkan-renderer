/// \file
/// \brief

//
// Created by Joshua Lowe on 12/15/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_PIPELINEBUILDER_H
#define VULKANRENDERER_PIPELINEBUILDER_H

#include "../core/Pipeline.h"

namespace vkf::core // Forward declaration
{
class Device;
} // namespace vkf::core

namespace vkf::rendering
{

class PipelineBuilder
{
  public:
    explicit PipelineBuilder() = default;                          // Default constructor
    PipelineBuilder(const PipelineBuilder &) = default;            // Default copy constructor
    PipelineBuilder(PipelineBuilder &&) noexcept = default;        // Default move constructor
    PipelineBuilder &operator=(const PipelineBuilder &) = default; // Default copy assignment operator
    PipelineBuilder &operator=(PipelineBuilder &&) = default;      // Default move assignment operator
    ~PipelineBuilder() = default;                                  // Default destructor

    // Setters for each state of the pipeline
    PipelineBuilder &setShaderStageCreateInfos(const std::vector<vk::PipelineShaderStageCreateInfo> &infos);
    PipelineBuilder &setVertexInputCreateInfo(const vk::PipelineVertexInputStateCreateInfo &info);
    PipelineBuilder &setInputAssemblyCreateInfo(const vk::PipelineInputAssemblyStateCreateInfo &info);
    PipelineBuilder &setPipelineViewportStateCreateInfo(const vk::PipelineViewportStateCreateInfo &info);
    PipelineBuilder &setRasterizerCreateInfo(const vk::PipelineRasterizationStateCreateInfo &info);
    PipelineBuilder &setMultisamplingCreateInfo(const vk::PipelineMultisampleStateCreateInfo &info);
    PipelineBuilder &setDepthStencilCreateInfo(const vk::PipelineDepthStencilStateCreateInfo &info);
    PipelineBuilder &setColorBlendingCreateInfo(const vk::PipelineColorBlendStateCreateInfo &info);
    PipelineBuilder &setDynamicStateCreateInfo(const vk::PipelineDynamicStateCreateInfo &info);
    PipelineBuilder &setPipelineLayout(const vk::PipelineLayout &layout);
    PipelineBuilder &setRenderPass(const vk::RenderPass &pass);

    // Build method that uses the current state to create and return a new Pipeline
    core::Pipeline build(const core::Device &device);

  private:
    core::PipelineState state;
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_PIPELINEBUILDER_H