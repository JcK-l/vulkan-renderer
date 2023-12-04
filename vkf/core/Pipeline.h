////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Pipeline.h
/// \brief This file declares the Pipeline class which is used for managing Vulkan pipelines.
///
/// The Pipeline class is part of the vkf::core namespace. It provides functionality to get the handle to the pipeline.
/// Currently, this class only creates a trivial pipeline, but it is planned to be expanded in the future.
///
/// \author Joshua Lowe
/// \date 12/3/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_PIPELINE_H
#define VULKANRENDERER_PIPELINE_H

namespace vkf::core
{

// Forward declarations
class Device;
class RenderPass;

///
/// \class Pipeline
/// \brief This class manages Vulkan pipelines.
///
/// Currently, this class only creates a trivial pipeline, but it is planned to be expanded in the future.
/// It provides functionality to get the handle to the pipeline.
///
class Pipeline
{
  public:
    ///
    /// \brief Constructor for the Pipeline class.
    ///
    /// \param device The Vulkan device.
    /// \param renderPass The render pass.
    ///
    Pipeline(const Device &device, const RenderPass &renderPass);

    [[nodiscard]] const vk::raii::Pipeline &getHandle() const;

  private:
    vk::raii::PipelineLayout pipelineLayout{VK_NULL_HANDLE};
    vk::raii::Pipeline handle{VK_NULL_HANDLE};
};

} // namespace vkf::core

#endif // VULKANRENDERER_PIPELINE_H