////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Buffer.cpp
/// \brief This file implements the Buffer class which is used for managing Vulkan buffers.
///
/// The Buffer class is part of the vkf::core namespace. It provides functionality to get the handle to the buffer,
/// map memory to the buffer, and unmap memory from the buffer.
///
/// \author Joshua Lowe
/// \date 12/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Buffer.h"
#include "CommandPool.h"
#include "Device.h"

namespace vkf::core
{

Buffer::Buffer(const Device &device, vk::BufferCreateInfo createInfo, VmaAllocationCreateFlags allocationFlags)
    : device{device}
{
    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocationCreateInfo.flags = allocationFlags;

    persistentMapped = (allocationFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;

    VmaAllocationInfo allocationInfo{};
    auto result = vmaCreateBuffer(device.getVmaAllocator(), reinterpret_cast<const VkBufferCreateInfo *>(&createInfo),
                                  &allocationCreateInfo, &handle, &allocation, &allocationInfo);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create buffer"};
    }

    if (persistentMapped)
    {
        mappedData = allocationInfo.pMappedData;
    }
}

Buffer::Buffer(Buffer &&other) noexcept
    : device{other.device}, handle{other.handle}, allocation{other.allocation},
      persistentMapped{other.persistentMapped}, mapped{other.mapped}, mappedData{other.mappedData}, size{other.size}
{
    // Invalidate the source object
    other.handle = VK_NULL_HANDLE;
    other.allocation = nullptr;
    other.mappedData = nullptr;
    other.mapped = false;
    other.size = 0;
}

Buffer::~Buffer()
{
    if (allocation && handle)
    {
        if (mapped)
        {
            unmapMemory();
        }
        vmaDestroyBuffer(device.getVmaAllocator(), handle, allocation);
        handle = VK_NULL_HANDLE;
    }
}

void Buffer::mapMemory()
{
    assert(!persistentMapped && "Cannot map persistent mapped memory");
    assert(!mapped && "Memory is already mapped");
    VkResult result = vmaMapMemory(device.getVmaAllocator(), allocation, &mappedData);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to map memory");
    }
    mapped = true;
}

void Buffer::unmapMemory()
{
    assert(!persistentMapped && "Cannot unmap persistent mapped memory");
    assert(mapped && "Memory is not mapped");
    vmaUnmapMemory(device.getVmaAllocator(), allocation);
}

void Buffer::updateData(const void *data, const uint32_t size, const uint32_t offset)
{
    this->size = size;
    const auto *srcData = static_cast<const uint8_t *>(data);
    auto *dstData = static_cast<uint8_t *>(mappedData) + offset;

    if (!persistentMapped)
    {
        mapMemory();
        dstData = static_cast<uint8_t *>(mappedData) + offset;
    }

    std::copy(srcData, srcData + size, dstData);

    if (!persistentMapped)
    {
        unmapMemory();
    }
}

void Buffer::copyBuffer(const Buffer &srcBuffer)
{
    auto &cmd = device.getCommandBuffers()->at(0);

    cmd.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    vk::BufferCopy copyRegion{.srcOffset = 0, .dstOffset = 0, .size = srcBuffer.getSize()};
    cmd.copyBuffer(srcBuffer.getBuffer(), this->getBuffer(), copyRegion);
    cmd.end();

    auto &queue = device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer).getHandle();
    queue.submit(vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &(*cmd)}, nullptr);
    queue.waitIdle();
}

vk::Buffer Buffer::getBuffer() const
{
    return {handle};
}

uint32_t Buffer::getSize() const
{
    return size;
}

} // namespace vkf::core