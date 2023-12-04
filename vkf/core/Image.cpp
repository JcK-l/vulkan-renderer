////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Image.cpp
/// \brief This file implements the Image class which is used for managing Vulkan images.
///
/// The Image class is part of the vkf::core namespace. It provides functionality to get the handle to the image,
/// map memory to the image, and unmap memory from the image.
///
/// \author Joshua Lowe
/// \date 12/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Image.h"
#include "Device.h"
#include "Queue.h"

namespace vkf::core
{

Image::Image(const Device &device, vk::ImageCreateInfo createInfo, VmaAllocationCreateFlagBits allocationFlags)
    : device{device}, createInfo{createInfo}
{
    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocationCreateInfo.flags = allocationFlags;

    auto result = vmaCreateImage(device.getVmaAllocator(), reinterpret_cast<const VkImageCreateInfo *>(&createInfo),
                                 &allocationCreateInfo, &handle, &allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create image"};
    }
}

Image::~Image()
{
    if (allocation && handle)
    {
        vmaDestroyImage(device.getVmaAllocator(), handle, allocation);
        handle = VK_NULL_HANDLE;
    }
}

vk::raii::ImageView Image::createImageView() const
{
    vk::ImageViewType viewType;
    switch (createInfo.imageType)
    {
    case vk::ImageType::e1D:
        viewType = vk::ImageViewType::e1D;
        break;
    case vk::ImageType::e2D:
        viewType = vk::ImageViewType::e2D;
        break;
    case vk::ImageType::e3D:
        viewType = vk::ImageViewType::e3D;
        break;
    default:
        throw std::runtime_error("Unsupported image type");
    }

    vk::ImageViewCreateInfo imageViewCreateInfo{.image = handle,
                                                .viewType = viewType,
                                                .format = createInfo.format,
                                                .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                                                                     .baseMipLevel = 0,
                                                                     .levelCount = createInfo.mipLevels,
                                                                     .baseArrayLayer = 0,
                                                                     .layerCount = createInfo.arrayLayers}};

    return vk::raii::ImageView{device.getHandle(), imageViewCreateInfo};
}

void Image::mapMemory(void **data)
{
    VkResult result = vmaMapMemory(device.getVmaAllocator(), allocation, data);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to map memory");
    }
}

void Image::unmapMemory()
{
    vmaUnmapMemory(device.getVmaAllocator(), allocation);
}

} // namespace vkf::core