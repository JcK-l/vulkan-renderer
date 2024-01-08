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
#include "Buffer.h"
#include "Device.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vkf::core
{

Image::Image(const Device &device, vk::ImageCreateInfo createInfo, VmaAllocationCreateFlags allocationFlags)
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
    currentLayout = vk::ImageLayout::eUndefined;
}

Image::Image(Image &&other) noexcept
    : device{other.device}, mapped{other.mapped}, mappedData{other.mappedData}, createInfo{other.createInfo},
      currentLayout{other.currentLayout}, allocation{other.allocation}, handle{other.handle},
      imageView{std::move(other.imageView)}, sampler{std::move(other.sampler)}
{
    // Invalidate the source object
    other.mapped = false;
    other.mappedData = nullptr;
    other.allocation = VK_NULL_HANDLE;
    other.handle = VK_NULL_HANDLE;
}

Image::~Image()
{
    if (allocation && handle)
    {
        if (mapped)
        {
            unmapMemory();
        }
        vmaDestroyImage(device.getVmaAllocator(), handle, allocation);
        handle = VK_NULL_HANDLE;
    }
}

vk::ImageView Image::getImageView(vk::ImageAspectFlags aspectFlags)
{
    if (!*imageView || aspectFlags != currentAspectFlags)
    {
        createImageView(aspectFlags);
    }
    return *imageView;
}

void Image::createImageView(vk::ImageAspectFlags aspectFlags)
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
                                                .subresourceRange = {.aspectMask = aspectFlags,
                                                                     .baseMipLevel = 0,
                                                                     .levelCount = createInfo.mipLevels,
                                                                     .baseArrayLayer = 0,
                                                                     .layerCount = createInfo.arrayLayers}};

    imageView = vk::raii::ImageView{device.getHandle(), imageViewCreateInfo};
    currentAspectFlags = aspectFlags;
}

vk::Sampler Image::getSampler()
{
    if (!*sampler)
    {
        createSampler();
    }
    return *sampler;
}

void Image::createSampler()
{
    vk::SamplerCreateInfo samplerInfo{.magFilter = vk::Filter::eLinear,
                                      .minFilter = vk::Filter::eLinear,
                                      .mipmapMode = vk::SamplerMipmapMode::eLinear,
                                      .addressModeU = vk::SamplerAddressMode::eRepeat,
                                      .addressModeV = vk::SamplerAddressMode::eRepeat,
                                      .addressModeW = vk::SamplerAddressMode::eRepeat,
                                      .anisotropyEnable = VK_TRUE,
                                      .maxAnisotropy = 16,
                                      .compareEnable = VK_FALSE,
                                      .compareOp = vk::CompareOp::eAlways,
                                      .borderColor = vk::BorderColor::eIntOpaqueBlack,
                                      .unnormalizedCoordinates = VK_FALSE};

    sampler = vk::raii::Sampler{device.getHandle(), samplerInfo};
}

void Image::mapMemory()
{
    assert(!mapped && "Memory is already mapped");
    VkResult result = vmaMapMemory(device.getVmaAllocator(), allocation, &mappedData);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to map memory");
    }
    mapped = true;
}

void Image::unmapMemory()
{
    assert(mapped && "Memory is not mapped");
    vmaUnmapMemory(device.getVmaAllocator(), allocation);
}

void Image::copyBufferToImage(const Buffer &srcBuffer)
{
    auto &cmd = device.getCommandBuffers()->at(0);

    cmd.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    vk::BufferImageCopy copyRegion{.bufferOffset = 0,
                                   .bufferRowLength = 0,
                                   .bufferImageHeight = 0,
                                   .imageSubresource =
                                       {
                                           .aspectMask = vk::ImageAspectFlagBits::eColor,
                                           .mipLevel = 0,
                                           .baseArrayLayer = 0,
                                           .layerCount = 1,
                                       },
                                   .imageOffset = vk::Offset3D{0, 0, 0},
                                   .imageExtent = vk::Extent3D{createInfo.extent.width, createInfo.extent.height, 1}};
    cmd.copyBufferToImage(srcBuffer.getBuffer(), handle, currentLayout, copyRegion);
    cmd.end();

    auto &queue = device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer).getHandle();
    queue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &(*cmd)}, nullptr);
    queue.waitIdle();
}

void Image::transitionImageLayout(vk::ImageLayout newLayout)
{
    auto &cmd = device.getCommandBuffers()->at(0);

    cmd.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    vk::ImageMemoryBarrier barrier{.oldLayout = currentLayout,
                                   .newLayout = newLayout,
                                   .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                   .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                   .image = handle,
                                   .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor,
                                                        .baseMipLevel = 0,
                                                        .levelCount = createInfo.mipLevels,
                                                        .baseArrayLayer = 0,
                                                        .layerCount = createInfo.arrayLayers}};

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (currentLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (currentLayout == vk::ImageLayout::eTransferDstOptimal &&
             newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }

    cmd.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags{}, {}, {}, {barrier});

    cmd.end();

    currentLayout = newLayout;

    auto &queue = device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer).getHandle();
    queue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &(*cmd)}, nullptr);
    queue.waitIdle();
}

} // namespace vkf::core