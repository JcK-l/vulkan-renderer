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
#include "Device.h"

namespace vkf::core
{

Buffer::Buffer(const Device &device, vk::BufferCreateInfo createInfo, VmaMemoryUsage memoryUsage) : device{device}
{
    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = memoryUsage;

    auto result = vmaCreateBuffer(device.getVmaAllocator(), reinterpret_cast<const VkBufferCreateInfo *>(&createInfo),
                                  &allocationCreateInfo, reinterpret_cast<VkBuffer *>(&handle), &allocation, nullptr);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create buffer"};
    }
}

Buffer::~Buffer()
{
    if (allocation)
    {
        vmaDestroyBuffer(device.getVmaAllocator(), *handle, allocation);
    }
}

const vk::raii::Buffer &Buffer::getHandle() const
{
    return handle;
}

void Buffer::mapMemory(void **data)
{
    VkResult result = vmaMapMemory(device.getVmaAllocator(), allocation, data);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to map memory");
    }
}

void Buffer::unmapMemory()
{
    vmaUnmapMemory(device.getVmaAllocator(), allocation);
}

} // namespace vkf::core