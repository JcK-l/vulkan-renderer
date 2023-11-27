////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Queue.cpp
/// \brief This file implements the Queue class which is used for managing Vulkan queues.
///
/// The Queue class is part of the vkf::core namespace. It provides an interface for interacting with a Vulkan queue,
/// including getting the handle to the queue, the family index, the queue properties, and the queue index. It also
/// provides a method to check if the queue can present.
///
/// \author Joshua Lowe
/// \date 11/11/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Queue.h"
#include "../common/Log.h"

namespace vkf::core
{

Queue::Queue(vk::raii::Queue queue, uint32_t familyIndex, vk::QueueFamilyProperties properties,
             vk::Bool32 supportsPresent, uint32_t queueIndex)
    : handle{std::move(queue)}, familyIndex{familyIndex}, properties{properties}, supportsPresent{supportsPresent},
      queueIndex{queueIndex}
{
}

const vk::raii::Queue &Queue::getHandle() const
{
    return handle;
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