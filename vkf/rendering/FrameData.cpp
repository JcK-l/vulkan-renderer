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
#include "../core/CommandPool.h"
#include "../core/Device.h"

namespace vkf::rendering
{

FrameData::FrameData(const core::Device &device) : device{device}
{
    commandPool = std::make_unique<core::CommandPool>(
        device, vk::CommandPoolCreateInfo{
                    .flags = vk::CommandPoolCreateFlags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
                    .queueFamilyIndex =
                        device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics, vk::QueueFlags()).getFamilyIndex()});

    commandBuffersMap.emplace(commandPool->requestCommandBuffers(vk::CommandBufferLevel::ePrimary, 1));
    semaphore[0] = vk::raii::Semaphore{device.getHandle(), vk::SemaphoreCreateInfo{}};
    semaphore[1] = vk::raii::Semaphore{device.getHandle(), vk::SemaphoreCreateInfo{}};
    fence = vk::raii::Fence{device.getHandle(), vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled}};
}

FrameData::~FrameData() = default;

vk::raii::CommandBuffer *FrameData::getCommandBuffer()
{
    return &commandBuffersMap[0]->at(0);
}
const vk::raii::Semaphore &FrameData::getSemaphore(uint32_t index) const
{
    return semaphore[index];
}

const vk::raii::Fence &FrameData::getFence() const
{
    return fence;
}

} // namespace vkf::rendering