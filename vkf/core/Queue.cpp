/// \file
/// \brief

//
// Created by Joshua Lowe on 11/11/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Queue.h"
#include "../common/Log.h"

namespace vkf::core
{

Queue::Queue(uint32_t familyIndex, vk::QueueFamilyProperties properties, vk::Bool32 supportsPresent,
             uint32_t queueIndex)
    : familyIndex{familyIndex}, properties{properties}, supportsPresent{supportsPresent}, queueIndex{queueIndex}
{
}

Queue::Queue(Queue &&other) noexcept
    : familyIndex{other.familyIndex}, properties{other.properties}, supportsPresent{other.supportsPresent},
      queueIndex{other.queueIndex}
{

    other.familyIndex = {};
    other.properties = vk::QueueFamilyProperties{};
    other.supportsPresent = VK_FALSE;
    other.queueIndex = 0;
}

uint32_t Queue::getFamilyIndex() const
{
    return familyIndex;
}

const vk::QueueFamilyProperties &Queue::getProperties() const
{
    return properties;
}

vk::Bool32 Queue::canPresent() const
{
    return supportsPresent;
}

uint32_t Queue::getQueueIndex() const
{
    return queueIndex;
}
} // namespace vkf::core