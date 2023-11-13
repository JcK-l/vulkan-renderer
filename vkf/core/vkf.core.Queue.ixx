/// \file
/// \brief

//
// Created by Joshua Lowe on 11/12/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include <vulkan/vulkan.hpp>

export module vkf.core.Queue;

export namespace vkf::core {
    class Queue {
    public:
        Queue(uint32_t familyIndex, vk::QueueFamilyProperties properties, vk::Bool32 canPresent,
              uint32_t queueIndex);

        Queue(const Queue &) = default;

        Queue(Queue &&other) noexcept;

        Queue &operator=(const Queue &) = delete;

        Queue &operator=(Queue &&) = delete;

    private:
        uint32_t familyIndex;
        vk::QueueFamilyProperties properties;
        vk::Bool32 canPresent;
        uint32_t queueIndex;
    };
} // namespace vkf::core

//@formatter:off
//module : private;
//@formatter:on

