////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file CommandPool.cpp
/// \brief This file implements the CommandPool class which is used to manage Vulkan command pools.
///
/// The CommandPool class is part of the vkf::core namespace. It provides functionality to get the handle to the command
/// pool and request command buffers.
///
/// \author Joshua Lowe
/// \date 11/20/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CommandPool.h"
#include "Device.h"

namespace vkf::core
{

CommandPool::CommandPool(const Device &device, vk::CommandPoolCreateInfo createInfo) : device{device}
{
    handle = vk::raii::CommandPool{device.getHandle(), createInfo};
}

const vk::raii::CommandPool &CommandPool::getHandle() const
{
    return handle;
}

std::pair<uint32_t, vk::raii::CommandBuffers *> CommandPool::requestCommandBuffers(vk::CommandBufferLevel level,
                                                                                   uint32_t count)
{
    commandBuffersMap.emplace(
        allocations, std::make_unique<vk::raii::CommandBuffers>(
                         device.getHandle(), vk::CommandBufferAllocateInfo{
                                                 .commandPool = *handle, .level = level, .commandBufferCount = count}));
    allocations++;
    return {allocations - 1, commandBuffersMap[allocations - 1].get()};
}

std::optional<vk::raii::CommandBuffers *> CommandPool::getCommandBuffers(uint32_t key) const
{
    auto it = commandBuffersMap.find(key);
    if (it != commandBuffersMap.end())
    {
        return it->second.get();
    }
    else
    {
        return std::nullopt;
    }
}

} // namespace vkf::core