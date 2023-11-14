/// \file
/// \brief

//
// Created by Joshua Lowe on 11/11/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Queue.h"
#include "../common/Log.h"

namespace vkf::core {

    Queue::Queue(uint32_t familyIndex, vk::QueueFamilyProperties properties, vk::Bool32 canPresent,
                 uint32_t queueIndex) : familyIndex{familyIndex}, properties{properties}, canPresent{canPresent},
                                        queueIndex{queueIndex} {
    }

    Queue::Queue(Queue &&other) noexcept: familyIndex{other.familyIndex}, properties{other.properties},
                                          canPresent{other.canPresent}, queueIndex{other.queueIndex} {

      other.familyIndex = {};
      other.properties = vk::QueueFamilyProperties{};
      other.canPresent = VK_FALSE;
      other.queueIndex = 0;
    }
} // namespace vkf::core