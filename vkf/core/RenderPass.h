////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RenderPass.h
/// \brief This file declares the RenderPass class which is used for managing Vulkan render passes.
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

#ifndef VULKANRENDERER_RENDERPASS_H
#define VULKANRENDERER_RENDERPASS_H

namespace vkf::core
{

// Forward declarations
class Device;

///
/// \class RenderPass
/// \brief This class manages Vulkan render passes.
///
/// It provides an interface for interacting with a Vulkan render pass, including getting the handle to the render pass.
/// It also provides methods for creating a render pass with subpass information and dependencies.
///
class RenderPass
{
  public:
    ///
    /// \brief Constructs a RenderPass object.
    ///
    /// This constructor creates a Vulkan render pass using the provided device, attachment descriptions,
    /// subpass information and dependencies.
    ///
    /// \param device The Vulkan device to use for creating the render pass.
    /// \param attachments The attachment descriptions to use for creating the render pass.
    /// \param subpassDescriptions The subpass information to use for creating the render pass.
    /// \param subpassDependencies The dependencies to use for creating the render pass.
    ///
    RenderPass(const core::Device &device, const std::vector<vk::AttachmentDescription> &attachments,
               const std::vector<vk::SubpassDescription> &subpassDescriptions,
               const std::vector<vk::SubpassDependency> &subpassDependencies);

    RenderPass(const RenderPass &) = delete;            // Deleted copy constructor
    RenderPass(RenderPass &&) noexcept = default;       // Default move constructor
    RenderPass &operator=(const RenderPass &) = delete; // Deleted copy assignment operator
    RenderPass &operator=(RenderPass &&) = delete;      // Default move assignment operator
    ~RenderPass() = default;                            // Default destructor

    [[nodiscard]] const vk::raii::RenderPass &getHandle() const;

  private:
    const core::Device &device;
    vk::raii::RenderPass handle{VK_NULL_HANDLE};

    std::vector<vk::AttachmentDescription> attachmentDescriptions{};
    std::vector<vk::SubpassDescription> subpassDescriptions{};
    std::vector<vk::SubpassDependency> subpassDependencies{};
};

} // namespace vkf::core

#endif // VULKANRENDERER_RENDERPASS_H