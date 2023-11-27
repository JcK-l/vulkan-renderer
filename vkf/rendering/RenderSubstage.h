////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RenderSubstage.h
/// \brief This file declares the RenderSubstage class which is used for managing Vulkan rendering sub-stages.
///
/// The RenderSubstage class is part of the vkf::rendering namespace. It provides an interface for interacting with
/// Vulkan rendering sub-stages, including a method for drawing using a provided command buffer.
///
/// \author Joshua Lowe
/// \date 11/21/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_RENDERSUBSTAGE_H
#define VULKANRENDERER_RENDERSUBSTAGE_H

namespace vkf::rendering
{

class RenderSubstage
{
  public:
    RenderSubstage();

    RenderSubstage(const RenderSubstage &) = delete;            // Deleted copy constructor
    RenderSubstage(RenderSubstage &&) noexcept = default;       // Default move constructor
    RenderSubstage &operator=(const RenderSubstage &) = delete; // Deleted copy assignment operator
    RenderSubstage &operator=(RenderSubstage &&) = delete;      // Deleted move assignment operator
    ~RenderSubstage() = default;                                // Default destructor

    void draw(vk::CommandBuffer *cmd);

  private:
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_RENDERSUBSTAGE_H