/// \file
/// \brief

//
// Created by Joshua Lowe on 11/16/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_SWAPCHAIN_H
#define VULKANRENDERER_SWAPCHAIN_H

#include "../platform/Window.h"
#include "Device.h"

namespace vkf::core
{

/// \brief This class provides access to the swapchain
class Swapchain
{
  public:
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    Swapchain(Device &device, vk::raii::SurfaceKHR &surface, const platform::Window &window);

    ~Swapchain() = default;

    Swapchain(const Swapchain &) = delete;

    Swapchain &operator=(const Swapchain &) = delete;

    [[nodiscard]] const vk::raii::SwapchainKHR &getHandle() const;

    [[nodiscard]] const std::vector<vk::raii::ImageView> &getImageViews() const;

    void recreate();

  private:
    vk::SurfaceFormatKHR selectSwapSurfaceFormat();

    vk::PresentModeKHR selectSwapPresentMode();

    vk::Extent2D selectSwapExtent();

    void createSwapchain(vk::SwapchainKHR oldSwapchain = VK_NULL_HANDLE);

    void createImageViews();

    void cleanup();

    Device &device;
    const platform::Window &window;
    vk::raii::SurfaceKHR &surface;
    vk::raii::SwapchainKHR handle{VK_NULL_HANDLE};

    SwapChainSupportDetails supportDetails;

    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
};
} // namespace vkf::core

#endif // VULKANRENDERER_SWAPCHAIN_H
