////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PipelineBuilder.h
/// \brief This file declares the PipelineBuilder class which is used for building Vulkan pipelines.
///
/// The PipelineBuilder class is part of the vkf::rendering namespace. It provides functionality to set various states
/// of the pipeline and build a new Pipeline using these states.
///
/// \author Joshua Lowe
/// \date 12/15/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../core/Pipeline.h"

// Forward declaration
#include "../core/CoreFwd.h"

namespace vkf::rendering
{

///
/// \class PipelineBuilder
/// \brief Class for building Vulkan pipelines.
///
/// This class provides functionality to set various states of the pipeline and build a new Pipeline using these states.
///
class PipelineBuilder
{
  public:
    explicit PipelineBuilder() = default;                          ///< Default constructor
    PipelineBuilder(const PipelineBuilder &);                      ///<  Copy constructor
    PipelineBuilder(PipelineBuilder &&) noexcept;                  ///<  Move constructor
    PipelineBuilder &operator=(const PipelineBuilder &) = default; ///< Default copy assignment operator
    PipelineBuilder &operator=(PipelineBuilder &&) = default;      ///< Default move assignment operator
    ~PipelineBuilder(); ///< Implementation in PipelineBuilder.cpp because of std::shared_ptr forward declaration

    // Setters for each state of the pipeline
    PipelineBuilder &setShaderStageCreateInfos(const core::Device &device, core::Shader &shader);
    PipelineBuilder &setVertexInputCreateInfo(const vk::PipelineVertexInputStateCreateInfo &info,
                                              vk::VertexInputBindingDescription &bindingDescription,
                                              std::vector<vk::VertexInputAttributeDescription> &attributeDescriptions);
    PipelineBuilder &setInputAssemblyCreateInfo(const vk::PipelineInputAssemblyStateCreateInfo &info);
    PipelineBuilder &setPipelineViewportStateCreateInfo(const vk::PipelineViewportStateCreateInfo &info);
    PipelineBuilder &setRasterizerCreateInfo(const vk::PipelineRasterizationStateCreateInfo &info);
    PipelineBuilder &setMultisamplingCreateInfo(const vk::PipelineMultisampleStateCreateInfo &info);
    PipelineBuilder &setDepthStencilCreateInfo(const vk::PipelineDepthStencilStateCreateInfo &info);
    PipelineBuilder &setColorBlendingCreateInfo(const vk::PipelineColorBlendStateCreateInfo &info,
                                                vk::PipelineColorBlendAttachmentState &attachment);
    PipelineBuilder &setDynamicStateCreateInfo(const vk::PipelineDynamicStateCreateInfo &info,
                                               std::vector<vk::DynamicState> &states);
    PipelineBuilder &setPipelineLayout(const vk::PipelineLayout &layout);
    PipelineBuilder &setRenderPass(const vk::RenderPass &pass);

    core::Pipeline build(const core::Device &device);

  private:
    core::PipelineState state;

    // hold state for later Pipeline construction
    std::shared_ptr<core::Shader> pipelineShader;
    std::shared_ptr<vk::PipelineColorBlendAttachmentState> colorBlendAttachment;
    std::shared_ptr<std::vector<vk::DynamicState>> dynamicStates;
    std::shared_ptr<vk::VertexInputBindingDescription> vertexInputBindingDescription;
    std::shared_ptr<std::vector<vk::VertexInputAttributeDescription>> vertexInputAttributeDescriptions;
};

} // namespace vkf::rendering