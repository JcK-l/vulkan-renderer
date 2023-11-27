////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RenderManager.h
/// \brief This file declares the RenderManager class which is used for managing Vulkan rendering operations.
///
/// The RenderManager class is part of the vkf::rendering namespace. It provides an interface for interacting with
/// Vulkan rendering operations, including beginning and ending frames and render passes, drawing, and managing the
/// lifecycle of a Renderer object.
///
/// \author Joshua Lowe
/// \date 11/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_RENDERMANAGER_H
#define VULKANRENDERER_RENDERMANAGER_H

namespace vkf::platform // Forward declarations
{
class Window;
} // namespace vkf::platform

namespace vkf::core // Forward declarations
{
class Swapchain;
class RenderPass;
class Framebuffer;
class Device;
} // namespace vkf::core

namespace vkf::rendering
{

// Forward declarations
class FrameData;
class Renderer;

///
/// \class RenderManager
/// \brief This class manages Vulkan rendering operations.
///
/// It provides an interface for interacting with Vulkan rendering operations, including beginning and ending frames and
/// render passes, drawing, and managing the lifecycle of a Renderer object.
///
class RenderManager
{
  public:
    ///
    /// \brief Constructs a RenderManager object.
    ///
    /// This constructor creates a RenderManager using the provided device, renderer, surface, and window.
    ///
    /// \param device The Vulkan device to use for creating the RenderManager.
    /// \param renderer The Renderer to use for creating the RenderManager.
    /// \param surface The Vulkan surface to use for creating the RenderManager.
    /// \param window The window to use for creating the RenderManager.
    ///
    RenderManager(const core::Device &device, platform::Window &window, std::unique_ptr<core::Swapchain> inputSwapchain,
                  std::unique_ptr<Renderer> inputRenderer);

    RenderManager(const RenderManager &) = delete;            // Deleted copy constructor
    RenderManager(RenderManager &&) noexcept = default;       // Default move constructor
    RenderManager &operator=(const RenderManager &) = delete; // Deleted copy assignment operator
    RenderManager &operator=(RenderManager &&) = delete;      // Deleted move assignment operator
    ~RenderManager(); // Implementation in RenderManager.cpp because of std::unique_ptr forward declaration

    void beginFrame();
    void endFrame();

    void beginRenderPass();
    void endRenderPass();

    void draw();

    static constexpr uint32_t framesInFlight{3};

  private:
    void createFrameData();
    void createFramebuffers();

    bool recreateSwapchain();

    const core::Device &device;
    platform::Window &window;

    std::unique_ptr<Renderer> renderer;
    core::RenderPass *renderPass;

    std::unique_ptr<core::Swapchain> swapchain;

    std::vector<std::unique_ptr<FrameData>> frameData;
    std::vector<std::unique_ptr<core::Framebuffer>> framebuffers;
    vk::raii::CommandBuffer *activeCommandBuffer{nullptr};

    bool frameActive{false};

    uint32_t activeFrame{0};
    uint32_t imageIndex{0};
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_RENDERMANAGER_H