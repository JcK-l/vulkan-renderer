/// \file
/// \brief

//
// Created by Joshua Lowe on 11/16/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Swapchain.h"
#include "../common/Log.h"
#include "../platform/Window.h"

namespace vkf::core
{

Swapchain::Swapchain(Device &device, vk::raii::SurfaceKHR &surface, const platform::Window &window)
    : device{device}, surface{surface}, window{window}
{
    try
    {
        auto &gpu = device.getPhysicalDevice();
        supportDetails.formats = gpu.getHandle().getSurfaceFormatsKHR(*surface);
        supportDetails.presentModes = gpu.getHandle().getSurfacePresentModesKHR(*surface);
        supportDetails.capabilities = gpu.getHandle().getSurfaceCapabilitiesKHR(*surface);

        createSwapchain();
        LOG_INFO("Created swapchain");
        createImageViews();
        LOG_INFO("Created {} swapchain image views", imageViews.size());
    }
    catch (vk::SystemError &err)
    {
        LOG_ERROR("vk::SystemError: {}", err.what());
        exit(-1);
    }
    catch (std::exception &err)
    {
        LOG_ERROR("std::exception: {}", err.what());
        exit(-1);
    }
    catch (...)
    {
        LOG_ERROR("unknown error");
        exit(-1);
    }
}

const vk::raii::SwapchainKHR &Swapchain::getHandle() const
{
    return handle;
}

const std::vector<vk::raii::ImageView> &Swapchain::getImageViews() const
{
    return imageViews;
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
    LOG_INFO("Recreated swapchain");
    createImageViews();
    LOG_INFO("Recreated {} swapchain image views", imageViews.size());
}

void Swapchain::createSwapchain(vk::SwapchainKHR oldSwapchain)
{
    auto surfaceFormat = selectSwapSurfaceFormat();
    auto presentMode = selectSwapPresentMode();
    auto extent = selectSwapExtent();

    auto preTransform = (supportDetails.capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity)
                            ? vk::SurfaceTransformFlagBitsKHR::eIdentity
                            : supportDetails.capabilities.currentTransform;

    uint32_t minImageCount =
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
}

void Swapchain::createImageViews()
{
    auto surfaceFormat = selectSwapSurfaceFormat();
    images = handle.getImages();
    imageViews.clear();
    imageViews.reserve(images.size());
    vk::ImageViewCreateInfo createInfo{.viewType = vk::ImageViewType::e2D,
                                       .format = surfaceFormat.format,
                                       .subresourceRange = {
                                           .aspectMask = vk::ImageAspectFlagBits::eColor,
                                           .baseMipLevel = 0,
                                           .levelCount = 1,
                                           .baseArrayLayer = 0,
                                           .layerCount = 1,
                                       }};
    for (const auto &image : images)
    {
        createInfo.image = image;
        imageViews.emplace_back(device.getHandle(), createInfo);
    }
}

void Swapchain::cleanup()
{
    imageViews.clear();
}

vk::SurfaceFormatKHR Swapchain::selectSwapSurfaceFormat()
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

vk::PresentModeKHR Swapchain::selectSwapPresentMode()
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

vk::Extent2D Swapchain::selectSwapExtent()
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
} // namespace vkf::core