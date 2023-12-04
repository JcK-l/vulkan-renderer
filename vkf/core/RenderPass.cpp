////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RenderPass.cpp
/// \brief This file implements the RenderPass class which is used for managing Vulkan render passes.
///
/// The RenderPass class is part of the vkf::core namespace. It provides an interface for interacting with a Vulkan
/// render pass, including getting the handle to the render pass. It also provides methods for creating a render pass
/// with subpass information and dependencies.
///
/// \author Joshua Lowe
/// \date 11/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RenderPass.h"
#include "../common/Log.h"
#include "../rendering/Renderer.h"
#include "Device.h"

namespace vkf::core
{

RenderPass::RenderPass(const core::Device &device, const std::vector<vk::AttachmentDescription> &attachments,
                       const std::vector<vk::SubpassDescription> &subpassDescriptions,
                       const std::vector<vk::SubpassDependency> &subpassDependencies)
    : device{device}
{

    LOG_INFO("dependencies: {}", subpassDependencies.size())
    auto createInfo = vk::RenderPassCreateInfo{.flags = vk::RenderPassCreateFlags{},
                                               .attachmentCount = static_cast<uint32_t>(attachments.size()),
                                               .pAttachments = attachments.data(),
                                               .subpassCount = static_cast<uint32_t>(subpassDescriptions.size()),
                                               .pSubpasses = subpassDescriptions.data(),
                                               .dependencyCount = static_cast<uint32_t>(subpassDependencies.size()),
                                               .pDependencies = subpassDependencies.data()};

    handle = vk::raii::RenderPass{device.getHandle(), createInfo};
    LOG_INFO("Created RenderPass")
}

const vk::raii::RenderPass &RenderPass::getHandle() const
{
    return handle;
}
} // namespace vkf::core