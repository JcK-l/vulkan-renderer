////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Renderer.h
/// \brief This file declares the Renderer class which is used for managing Vulkan rendering operations.
///
/// The Renderer class is part of the vkf::rendering namespace. It provides an interface for interacting with Vulkan
/// rendering operations.
///
/// \author Joshua Lowe
/// \date 11/20/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_RENDERER_H
#define VULKANRENDERER_RENDERER_H

#include "../core/Image.h"
#include "RenderSource.h"
#include "RenderSubstage.h"

// Forward declarations for classes in the vkf::core namespace
namespace vkf::core
{
class Device;
class RenderPass;
class Framebuffer;
} // namespace vkf::core

namespace vkf::rendering
{

// Forward declarations for classes in the vkf::rendering namespace
class FrameData;

///
/// \struct RenderOptions
/// \brief This struct stores options for rendering.
///
/// It includes a clear value and the number of subpasses.
/// It also includes a vector of attachment descriptions.
///
struct RenderOptions
{
    std::vector<vk::ClearValue> clearValues;            // The clear value for the render
    uint32_t numSubpasses;                              // The number of subpasses for the render
    std::vector<vk::AttachmentDescription> attachments; // The attachment descriptions for the render
    bool useDepth;                                      // Whether or not to use depth
};

///
/// \class Renderer
/// \brief This class manages Vulkan rendering operations.
///
/// It provides an interface for interacting with Vulkan rendering operations. It includes methods for getting the
/// render pass and clear value, as well as methods for updating framebuffers and drawing.
///
class Renderer
{
  public:
    ///
    /// \brief Constructor for the Renderer class.
    ///
    /// This constructor creates a Vulkan renderer using the provided device, swapchain, and render options.
    ///
    /// \param device The Vulkan device.
    /// \param renderOptions The render options.
    /// \param renderSource The render source.
    ///
    Renderer(const core::Device &device, RenderOptions inputRenderOptions, std::shared_ptr<RenderSource> renderSource);

    Renderer(const Renderer &) = delete;            ///< Deleted copy constructor
    Renderer(Renderer &&) noexcept = default;       ///< Default move constructor
    Renderer &operator=(const Renderer &) = delete; ///< Deleted copy assignment operator
    Renderer &operator=(Renderer &&) = delete;      ///< Deleted move assignment operator
    ~Renderer();                                    ///< Destructor for the Renderer class

    [[nodiscard]] core::RenderPass *getRenderPass() const;
    [[nodiscard]] vk::RenderPassBeginInfo getRenderPassBeginInfo(uint32_t imageIndex) const;
    [[nodiscard]] vk::Extent2D getFramebufferExtent() const;

    void addRenderSubstage(std::unique_ptr<RenderSubstage> renderSubstage);

    void updateFramebuffers();

    void draw(vk::raii::CommandBuffer *cmd);

  private:
    void createFramebuffers(std::vector<vk::ImageView> imageViews);
    void createDepthImages();
    void createDepthImageViews();
    vk::ImageView getDepthImageView(uint32_t index);

    const core::Device &device;

    std::unique_ptr<core::RenderPass> renderPass;
    RenderOptions renderOptions;

    std::vector<std::unique_ptr<core::Framebuffer>> framebuffers;
    std::vector<std::unique_ptr<RenderSubstage>> renderSubstages{};

    vk::Extent2D framebufferExtent;

    std::vector<core::Image> depthImages;
    std::vector<vk::raii::ImageView> depthImageViews;

    std::shared_ptr<RenderSource> renderSource;
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_RENDERER_H