////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Swapchain.h
/// \brief This file declares the Swapchain class which is used for managing Vulkan swapchains.
///
/// The Swapchain class is part of the vkf::core namespace. It provides an interface for interacting with a Vulkan
/// swapchain, including getting the handle to the swapchain, the image views, and acquiring the next image. It also
/// provides a method for recreating the swapchain.
///
/// \author Joshua Lowe
/// \date 11/16/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_SWAPCHAIN_H
#define VULKANRENDERER_SWAPCHAIN_H

#include "../rendering/RenderSource.h"

namespace vkf::platform // Forward declarations
{
class Window;
} // namespace vkf::platform

namespace vkf::core
{

// Forward declarations
class Device;
class PhysicalDevice;

///
/// \class Swapchain
/// \brief This class manages Vulkan swapchains.
///
/// It provides an interface for interacting with a Vulkan swapchain, including getting the handle to the swapchain, the
/// image views, and acquiring the next image. It also provides a method for recreating the swapchain.
/// Swapchain is a subclass of RenderSource and therefore provides methods for getting the image views and image count.
///
class Swapchain : public rendering::RenderSource
{
  public:
    ///
    /// \struct SwapChainSupportDetails
    /// \brief This struct stores the capabilities, formats, and present modes of the swapchain.
    ///
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    ///
    /// \brief Constructs a Swapchain object.
    ///
    /// This constructor creates a Vulkan swapchain using the provided device, surface, and window.
    ///
    /// \param device The Vulkan device to use for creating the swapchain.
    /// \param surface The Vulkan surface to use for creating the swapchain.
    /// \param window The window to use for creating the swapchain.
    ///
    Swapchain(const Device &device, const vk::raii::SurfaceKHR &surface, const platform::Window &window);

    Swapchain(const Swapchain &) = delete;            ///< Deleted copy constructor
    Swapchain(Swapchain &&) noexcept = default;       ///< Default move constructor
    Swapchain &operator=(const Swapchain &) = delete; ///< Deleted copy assignment operator
    Swapchain &operator=(Swapchain &&) = delete;      ///< Deleted move assignment operator
    ~Swapchain() override = default;                  ///< Default destructor

    [[nodiscard]] const vk::raii::SwapchainKHR &getHandle() const;

    [[nodiscard]] std::vector<vk::ImageView> getImageViews() const override;
    [[nodiscard]] uint32_t getImageCount() const override;
    [[nodiscard]] vk::Extent2D getExtent() const override;
    [[nodiscard]] bool resetChanged() override;
    [[nodiscard]] uint32_t getMinImageCount() const;

    void recreate();

    std::pair<vk::Result, uint32_t> acquireNextImage(const vk::raii::Semaphore &imageAvailableSemaphore,
                                                     uint64_t timeout = std::numeric_limits<uint64_t>::max());

    [[nodiscard]] vk::SurfaceFormatKHR selectSwapSurfaceFormat() const;

  private:
    [[nodiscard]] vk::PresentModeKHR selectSwapPresentMode() const;
    [[nodiscard]] vk::Extent2D selectSwapExtent() const;

    void createImageViews();

    void createSwapchain(vk::SwapchainKHR oldSwapchain = VK_NULL_HANDLE);

    const Device &device;
    const PhysicalDevice &gpu;
    const vk::raii::SurfaceKHR &surface;
    const platform::Window &window;
    vk::raii::SwapchainKHR handle{VK_NULL_HANDLE};

    SwapChainSupportDetails supportDetails;
    vk::Extent2D extent;
    uint32_t minImageCount{0};
    bool changed{false};

    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
};
} // namespace vkf::core

#endif // VULKANRENDERER_SWAPCHAIN_H