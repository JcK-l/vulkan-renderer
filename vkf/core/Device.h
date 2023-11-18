/// \file
/// \brief

//
// Created by Joshua Lowe on 11/9/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_DEVICE_H
#define VULKANRENDERER_DEVICE_H

#include "Instance.h"
#include "PhysicalDevice.h"

#include "Queue.h"
#include <vk_mem_alloc.h>

namespace vkf::core
{

/// \brief This class provides access to device properties. It also creates a VmaAllocator
class Device
{
  public:
    Device(Instance &instance, vk::raii::SurfaceKHR &surface, const std::vector<const char *> &requiredExtensions = {});

    Device(const Device &) = delete;

    Device(Device &&) = delete;

    ~Device() = default;

    Device &operator=(const Device &) = delete;

    Device &operator=(Device &&) = delete;

    [[nodiscard]] const Queue &getQueue(uint32_t queueIndex, uint32_t familyIndex) const;

    [[nodiscard]] const Queue &getQueueWithFlags(uint32_t queueIndex, vk::QueueFlags includeFlags,
                                                 vk::QueueFlags excludeFlags = vk::QueueFlags()) const;

    [[nodiscard]] const Queue &getQueueWithPresent(uint32_t queueIndex,
                                                   vk::QueueFlags excludeFlags = vk::QueueFlags()) const;

    [[nodiscard]] bool hasQueue(uint32_t queueIndex, uint32_t familyIndex) const;

    [[nodiscard]] bool hasQueueWithFlags(uint32_t queueIndex, vk::QueueFlags includeFlags,
                                         vk::QueueFlags excludeFlags = vk::QueueFlags()) const;

    [[nodiscard]] bool hasQueueWithPresent(uint32_t queueIndex, vk::QueueFlags excludeFlags = vk::QueueFlags()) const;

    [[nodiscard]] VmaAllocator &getVmaAllocator();

    [[nodiscard]] const vk::raii::Device &getHandle() const;

    [[nodiscard]] const PhysicalDevice &getPhysicalDevice() const;

  private:
    std::vector<vk::DeviceQueueCreateInfo> createQueues();

    void createVmaAllocator(const Instance &instance, const PhysicalDevice &gpu);

    /// \brief Tries to enable all required device extensions
    /// \param requiredExtensions
    void validateExtensions(const std::vector<const char *> &requiredExtensions);

    /// \brief Enables device extension if it is in availableExtensions
    /// \param requiredExtensionName Name of extension
    /// \return IsExtensionEnabled
    bool enableExtension(const char *requiredExtensionName);

    std::vector<const char *> enabledExtensions;
    std::vector<vk::ExtensionProperties> availableExtensions;

    vk::raii::Device handle{VK_NULL_HANDLE};

    PhysicalDevice &gpu;
    vk::raii::SurfaceKHR &surface;
    VmaAllocator vmaAllocator{VK_NULL_HANDLE};
    std::vector<std::vector<Queue>> queues;
};
} // namespace vkf::core

#endif // VULKANRENDERER_DEVICE_H
