////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Swapchain.cpp
/// \brief This file implements the Swapchain class which is used for managing Vulkan swapchains.
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

#include "Swapchain.h"
#include "../common/Log.h"
#include "../platform/Window.h"
#include "Device.h"
#include "PhysicalDevice.h"

namespace vkf::core
{

Swapchain::Swapchain(const Device &device, const vk::raii::SurfaceKHR &surface, const platform::Window &window)
    : device{device}, gpu{device.getPhysicalDevice()}, surface{surface}, window{window}
{
    createSwapchain();
    LOG_INFO("Created Swapchain")
}

const vk::raii::SwapchainKHR &Swapchain::getHandle() const
{
    return handle;
}

void Swapchain::recreate()
{
    auto [width, height] = window.getFramebufferSize();
    while (width == 0 || height == 0)
    {
        std::tie(width, height) = window.getFramebufferSize();
        window.waitEvents();
    }
    device.getHandle().waitIdle();

    createSwapchain(*handle);
    LOG_INFO("Recreated Swapchain")
}

void Swapchain::createSwapchain(vk::SwapchainKHR oldSwapchain)
{

    supportDetails.formats = gpu.getHandle().getSurfaceFormatsKHR(*surface);
    supportDetails.presentModes = gpu.getHandle().getSurfacePresentModesKHR(*surface);
    supportDetails.capabilities = gpu.getHandle().getSurfaceCapabilitiesKHR(*surface);

    auto surfaceFormat = selectSwapSurfaceFormat();
    auto presentMode = selectSwapPresentMode();
    extent = selectSwapExtent();

    auto preTransform = (supportDetails.capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
                            ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                            : supportDetails.capabilities.currentTransform;

    minImageCount =
        std::clamp(3u, supportDetails.capabilities.minImageCount, supportDetails.capabilities.maxImageCount);

    vk::CompositeAlphaFlagBitsKHR compositeAlpha;

    if (supportDetails.capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
    {
        compositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
    }
    else if (supportDetails.capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
    {
        compositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;
    }
    else if (supportDetails.capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
    {
        compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eInherit;
    }
    else
    {
        compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    }

    vk::SwapchainCreateInfoKHR createInfo{
        .surface = *surface,
        .minImageCount = minImageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = preTransform,
        .compositeAlpha = compositeAlpha,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = oldSwapchain,
    };

    handle = vk::raii::SwapchainKHR{device.getHandle(), createInfo};
    images = handle.getImages();
    createImageViews();
    changed = true;
}

std::vector<vk::ImageView> Swapchain::getImageViews() const
{
    std::vector<vk::ImageView> result;
    result.reserve(imageViews.size());
    for (auto i = 0u; i < images.size(); ++i)
    {
        result.emplace_back(*imageViews[i]);
    }
    return result;
}

void Swapchain::createImageViews()
{
    imageViews.clear();
    imageViews.reserve(images.size());
    for (auto &image : images)
    {
        auto surfaceFormat = selectSwapSurfaceFormat();
        vk::ImageViewCreateInfo createInfo{.image = image,
                                           .viewType = vk::ImageViewType::e2D,
                                           .format = surfaceFormat.format,
                                           .subresourceRange = {
                                               .aspectMask = vk::ImageAspectFlagBits::eColor,
                                               .baseMipLevel = 0,
                                               .levelCount = 1,
                                               .baseArrayLayer = 0,
                                               .layerCount = 1,
                                           }};

        imageViews.emplace_back(device.getHandle(), createInfo);
    }
}

vk::SurfaceFormatKHR Swapchain::selectSwapSurfaceFormat() const
{
    for (const auto &availableFormat : supportDetails.formats)
    {
        if (availableFormat.format == vk::Format::eR8G8B8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }
    return supportDetails.formats[0];
}

vk::PresentModeKHR Swapchain::selectSwapPresentMode() const
{
    for (const auto &availablePresentMode : supportDetails.presentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D Swapchain::selectSwapExtent() const
{
    if (supportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return supportDetails.capabilities.currentExtent;
    }
    else
    {
        auto [width, height] = window.getFramebufferSize();
        vk::Extent2D actualExtent{width, height};

        actualExtent.width = std::clamp(actualExtent.width, supportDetails.capabilities.minImageExtent.width,
                                        supportDetails.capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, supportDetails.capabilities.minImageExtent.height,
                                         supportDetails.capabilities.maxImageExtent.height);
        return actualExtent;
    }
}

std::pair<vk::Result, uint32_t> Swapchain::acquireNextImage(const vk::raii::Semaphore &imageAvailableSemaphore,
                                                            uint64_t timeout)
{
    return handle.acquireNextImage(timeout, *imageAvailableSemaphore);
}

vk::Extent2D Swapchain::getExtent() const
{
    return extent;
}

bool Swapchain::resetChanged()
{
    bool currentChanged = changed;
    changed = false;
    return currentChanged;
}

uint32_t Swapchain::getImageCount() const
{
    return images.size();
}

uint32_t Swapchain::getMinImageCount() const
{
    return minImageCount;
}

} // namespace vkf::core