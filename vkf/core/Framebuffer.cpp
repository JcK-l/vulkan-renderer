////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Framebuffer.cpp
/// \brief This file implements the Framebuffer class, which encapsulates a Vulkan framebuffer.
///
/// The Framebuffer class is part of the vkf::core namespace. It provides an interface for creating and managing a
/// Vulkan framebuffer. A framebuffer in Vulkan is a collection of specific attachments (images) that a render pass
/// instance uses.
///
/// \author Joshua Lowe
/// \date 11/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Framebuffer.h"
#include "../rendering/Renderer.h"
#include "Device.h"
#include "RenderPass.h"

namespace vkf::core
{

Framebuffer::Framebuffer(const Device &device, const RenderPass &renderPass,
                         const std::vector<vk::ImageView> &attachments, const vk::Extent2D &extent)
{
    handle = vk::raii::Framebuffer{device.getHandle(), vk::FramebufferCreateInfo{
                                                           .flags = vk::FramebufferCreateFlags{},
                                                           .renderPass = *renderPass.getHandle(),
                                                           .attachmentCount = static_cast<uint32_t>(attachments.size()),
                                                           .pAttachments = attachments.data(),
                                                           .width = extent.width,
                                                           .height = extent.height,
                                                           .layers = 1,
                                                       }};
}

const vk::raii::Framebuffer &Framebuffer::getHandle() const
{
    return handle;
}

} // namespace vkf::core