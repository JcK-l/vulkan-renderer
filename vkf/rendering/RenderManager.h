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

#pragma once

// Forward declarations
#include "../core/CoreFwd.h"
#include "../platform/PlatformFwd.h"
#include "../rendering/RenderingFwd.h"

namespace vkf::rendering
{

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
    /// \param window The window to use for creating the RenderManager.
    /// \param inputSwapchain The swapchain to use for creating the RenderManager.
    /// \param inputGui The GUI to use for creating the RenderManager.
    /// \param inputRenderers The renderers to use for creating the RenderManager.
    ///
    RenderManager(const core::Device &device, platform::Window &window, std::shared_ptr<core::Swapchain> inputSwapchain,
                  std::vector<std::unique_ptr<Renderer>> inputRenderers);

    RenderManager(const RenderManager &) = delete;            ///< Deleted copy constructor
    RenderManager(RenderManager &&) noexcept = default;       ///< Default move constructor
    RenderManager &operator=(const RenderManager &) = delete; ///< Deleted copy assignment operator
    RenderManager &operator=(RenderManager &&) = delete;      ///< Deleted move assignment operator
    ~RenderManager(); ///< Implementation in RenderManager.cpp because of std::unique_ptr forward declaration

    void beginFrame();
    void endFrame();

    void render();

    void syncFrameData();

    static constexpr uint32_t framesInFlight{3};

  private:
    void beginRenderPass(Renderer &renderer, uint32_t currentRenderPass);
    void endRenderPass(uint32_t currentRenderPass);
    void updateFrameBuffers();

    void createFrameData();

    bool recreateSwapchain();

    const core::Device &device;
    platform::Window &window;

    std::vector<std::unique_ptr<Renderer>> renderers;

    std::shared_ptr<core::Swapchain> swapchain;

    std::vector<std::unique_ptr<FrameData>> frameData;
    vk::raii::CommandBuffers *activeCommandBuffers{nullptr};

    bool frameActive{false};

    uint32_t activeFrame{0};
    uint32_t imageIndex{0};
};

} // namespace vkf::rendering