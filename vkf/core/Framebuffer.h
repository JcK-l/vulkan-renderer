////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Framebuffer.h
/// \brief This file declares the Framebuffer class, which encapsulates a Vulkan framebuffer.
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

#ifndef VULKANRENDERER_FRAMEBUFFER_H
#define VULKANRENDERER_FRAMEBUFFER_H

namespace vkf::core
{

// Forward declarations
class Device;
class RenderPass;

///
/// \class Framebuffer
/// \brief This class manages Vulkan framebuffers.
///
/// This class provides an interface for creating and managing a Vulkan framebuffer. A framebuffer in Vulkan is a
/// collection of specific attachments (images) that a render pass instance uses.
///
class Framebuffer
{
  public:
    ///
    /// \brief Constructs a Framebuffer object.
    ///
    /// This constructor creates a Vulkan framebuffer using the provided device, render pass, attachments and extent.
    ///
    /// \param device The Vulkan device to use for creating the framebuffer.
    /// \param renderPass The render pass to use for creating the framebuffer.
    /// \param attachments The attachments to use for creating the framebuffer.
    /// \param extent The extent of the framebuffer.
    ///
    Framebuffer(const Device &device, const RenderPass &renderPass, const std::vector<vk::ImageView> &attachments,
                const vk::Extent2D &extent);

    Framebuffer(const Framebuffer &) = delete;            ///< Deleted copy constructor
    Framebuffer(Framebuffer &&) noexcept = default;       ///< Default move constructor
    Framebuffer &operator=(const Framebuffer &) = delete; ///< Deleted copy assignment operator
    Framebuffer &operator=(Framebuffer &&) = delete;      ///< Deleted move assignment operator
    ~Framebuffer() = default;                             ///< Default destructor

    [[nodiscard]] const vk::raii::Framebuffer &getHandle() const;

  private:
    vk::raii::Framebuffer handle{VK_NULL_HANDLE};
};

} // namespace vkf::core

#endif // VULKANRENDERER_FRAMEBUFFER_H