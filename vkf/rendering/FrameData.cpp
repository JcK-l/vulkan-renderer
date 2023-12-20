////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file FrameData.cpp
/// \brief This file implements the FrameData class which is used for managing frame data in Vulkan.
///
/// The FrameData class is part of the vkf::rendering namespace. It provides an interface for interacting with frame
/// data, including getting the command buffer.
///
/// \author Joshua Lowe
/// \date 11/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FrameData.h"
#include "../common/Log.h"
#include "../core/CommandPool.h"
#include "../core/Device.h"

namespace vkf::rendering
{

FrameData::FrameData(const core::Device &device, uint32_t numRenderPasses) : device{device}
{
    commandPool = std::make_unique<core::CommandPool>(
        device, vk::CommandPoolCreateInfo{
                    .flags = vk::CommandPoolCreateFlags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
                    .queueFamilyIndex =
                        device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics, vk::QueueFlags()).getFamilyIndex()});

    commandBuffersMap.emplace(commandPool->requestCommandBuffers(vk::CommandBufferLevel::ePrimary, numRenderPasses));
    semaphore.emplace_back(device.getHandle(), vk::SemaphoreCreateInfo{});
    for (auto i = 0u; i < numRenderPasses - 1; ++i)
    {
        semaphore.emplace_back(device.getHandle(), vk::SemaphoreCreateInfo{});
    }
    semaphore.emplace_back(device.getHandle(), vk::SemaphoreCreateInfo{});

    for (auto i = 0u; i < numRenderPasses; ++i)
    {
        fences.emplace_back(device.getHandle(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
    }
}

FrameData::~FrameData() = default;

vk::raii::CommandBuffers *FrameData::getCommandBuffers()
{
    return commandBuffersMap[0];
}
const vk::raii::Semaphore &FrameData::getSemaphore(uint32_t index) const
{
    return semaphore[index];
}

void FrameData::refreshSemaphore(uint32_t index)
{
    semaphore[index] = vk::raii::Semaphore{device.getHandle(), vk::SemaphoreCreateInfo{}};
}

const vk::raii::Semaphore &FrameData::getLastSemaphore() const
{
    return semaphore[semaphore.size() - 1];
}

std::vector<vk::Fence> FrameData::getFences() const
{
    std::vector<vk::Fence> result;
    result.reserve(fences.size());
    for (auto &fence : fences)
        result.emplace_back(*fence);
    return result;
}

} // namespace vkf::rendering