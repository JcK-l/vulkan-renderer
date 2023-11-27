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

namespace vkf::core // Forward declarations
{
class Device;
class Swapchain;
class RenderPass;
} // namespace vkf::core

namespace vkf::rendering
{

// Forward declarations
class RenderSubstage;

///
/// \struct RenderTarget
/// \brief This struct stores information related to a render target.
///
/// It includes the extent of the render target, a vector of image views, and a vector of attachment descriptions.
///
struct RenderTarget
{
    vk::Extent2D extent;
    std::vector<vk::raii::ImageView> imageViews;
    std::vector<vk::AttachmentDescription> attachments;
};

///
/// \struct RenderOptions
/// \brief This struct stores options for rendering.
///
/// It includes a clear value and the number of subpasses.
///
struct RenderOptions
{
    vk::ClearValue clearValue;
    uint32_t numSubpasses;
};

///
/// \class Renderer
/// \brief This class manages Vulkan rendering operations.
///
/// It provides an interface for interacting with Vulkan rendering operations. It includes methods for getting the
/// render pass, clear value, and render target, as well as methods updating render targets and drawing.
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
    /// \param swapchain The Vulkan swapchain.
    /// \param renderOptions The render options.
    ///
    explicit Renderer(const core::Device &device, core::Swapchain *swapchain, RenderOptions renderOptions);

    Renderer(const Renderer &) = delete;            // Deleted copy constructor
    Renderer(Renderer &&) noexcept = default;       // Default move constructor
    Renderer &operator=(const Renderer &) = delete; // Deleted copy assignment operator
    Renderer &operator=(Renderer &&) = delete;      // Deleted move assignment operator
    ~Renderer(); // Implementation in Renderer.cpp because of std::unique_ptr forward declaration

    [[nodiscard]] core::RenderPass *getRenderPass() const;
    [[nodiscard]] vk::ClearValue *getClearValue() const;
    [[nodiscard]] const RenderTarget &getRenderTarget(uint32_t index);

    void draw(vk::CommandBuffer *cmd);

    void updateRenderTargets();

  private:
    void createRenderTargets();
    void createRenderSubstages();

    const core::Device &device;

    std::unique_ptr<core::RenderPass> renderPass;
    core::Swapchain *swapchain;
    std::unique_ptr<RenderOptions> renderOptions;
    std::vector<std::unique_ptr<RenderTarget>> renderTargets;
    std::vector<std::unique_ptr<RenderSubstage>> renderSubstages;
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_RENDERER_H