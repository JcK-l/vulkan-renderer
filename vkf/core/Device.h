////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Device.h
/// \brief This file declares the Device class which is used to manage Vulkan devices.
///
/// The Device class is part of the vkf::core namespace. It provides functionality to get queues and handle to the
/// device. It also provides methods to check if a queue exists and to get the VMA allocator.
///
/// \author Joshua Lowe
/// \date 11/9/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_DEVICE_H
#define VULKANRENDERER_DEVICE_H

#include "Queue.h"
#include <vk_mem_alloc.h>

namespace vkf::core
{

// Forward declarations
class Instance;
class PhysicalDevice;

///
/// \class Device
/// \brief This class is used to manage Vulkan devices.
///
/// This class provides functionality to get queues and handle to the device.
/// It also provides methods to check if a queue exists and to get the VMA allocator.
///
class Device
{
  public:
    ///
    /// \brief Constructor for the Device class.
    ///
    /// This constructor creates a Vulkan device using the provided instance, surface, and required extensions.
    ///
    /// \param instance The Vulkan instance.
    /// \param surface The Vulkan surface.
    /// \param requiredExtensions A vector of required extensions (optional).
    ///
    Device(Instance &instance, vk::raii::SurfaceKHR &surface, const std::vector<const char *> &requiredExtensions = {});

    Device(const Device &) = delete;            // Deleted copy constructor
    Device(Device &&) noexcept = default;       // Default move constructor
    Device &operator=(const Device &) = delete; // Deleted copy assignment operator
    Device &operator=(Device &&) = delete;      // Deleted move assignment operator
    ~Device();                                  // destructor

    ///
    /// \brief Getter for a queue.
    ///
    /// This function returns a queue with the given queue index and family index.
    ///
    /// \param queueIndex The index of the queue.
    /// \param familyIndex The family index of the queue.
    /// \return A const reference to the queue.
    ///
    [[nodiscard]] const Queue &getQueue(uint32_t queueIndex, uint32_t familyIndex) const;

    ///
    /// \brief Getter for a queue with flags.
    ///
    /// This function returns a queue with the given queue index that includes the specified flags and excludes the
    /// specified flags.
    ///
    /// \param queueIndex The index of the queue.
    /// \param includeFlags The flags that the queue must include.
    /// \param excludeFlags The flags that the queue must not include (optional).
    /// \return A const reference to the queue.
    ///
    [[nodiscard]] const Queue &getQueueWithFlags(uint32_t queueIndex, vk::QueueFlags includeFlags,
                                                 vk::QueueFlags excludeFlags = vk::QueueFlags()) const;

    ///
    /// \brief Getter for a queue with present support.
    ///
    /// This function returns a queue with the given queue index that supports presentation and excludes the specified
    /// flags.
    ///
    /// \param queueIndex The index of the queue.
    /// \param excludeFlags The flags that the queue must not include (optional).
    /// \return A const reference to the queue.
    ///
    [[nodiscard]] const Queue &getQueueWithPresent(uint32_t queueIndex,
                                                   vk::QueueFlags excludeFlags = vk::QueueFlags()) const;

    ///
    /// \brief Checks if a queue exists.
    ///
    /// This function checks if a queue with the given queue index and family index exists.
    ///
    /// \param queueIndex The index of the queue.
    /// \param familyIndex The family index of the queue.
    /// \return True if the queue exists, false otherwise.
    ///
    [[nodiscard]] bool hasQueue(uint32_t queueIndex, uint32_t familyIndex) const;

    ///
    /// \brief Checks if a queue with flags exists.
    ///
    /// This function checks if a queue with the given queue index that includes the specified flags and excludes the
    /// specified flags exists.
    ///
    /// \param queueIndex The index of the queue.
    /// \param includeFlags The flags that the queue must include.
    /// \param excludeFlags The flags that the queue must not include (optional).
    /// \return True if the queue exists, false otherwise.
    ///
    [[nodiscard]] bool hasQueueWithFlags(uint32_t queueIndex, vk::QueueFlags includeFlags,
                                         vk::QueueFlags excludeFlags = vk::QueueFlags()) const;

    ///
    /// \brief Checks if a queue with present support exists.
    ///
    /// This function checks if a queue with the given queue index that supports presentation and excludes the specified
    /// flags exists.
    ///
    /// \param queueIndex The index of the queue.
    /// \param excludeFlags The flags that the queue must not include (optional).
    /// \return True if the queue exists, false otherwise.
    ///
    [[nodiscard]] bool hasQueueWithPresent(uint32_t queueIndex, vk::QueueFlags excludeFlags = vk::QueueFlags()) const;

    [[nodiscard]] const VmaAllocator &getVmaAllocator() const;
    [[nodiscard]] const vk::raii::Device &getHandle() const;
    [[nodiscard]] const PhysicalDevice &getPhysicalDevice() const;

  private:
    std::vector<vk::DeviceQueueCreateInfo> createQueuesInfos();
    void createQueues();

    void createVmaAllocator(const Instance &instance, const PhysicalDevice &gpu);

    ///
    /// \brief Validates the required extensions.
    ///
    /// This function validates the required extensions by checking if they are available.
    /// If a required extension is not available, a runtime_error is thrown.
    ///
    /// \param requiredExtensions A vector of required extensions.
    ///
    void validateExtensions(const std::vector<const char *> &requiredExtensions);

    ///
    /// \brief Enables an extension.
    ///
    /// This function enables an extension by adding it to the enabledExtensions vector.
    /// If the extension is not available, it is not added to the vector and the function returns false.
    ///
    /// \param requiredExtensionName The name of the extension to enable.
    /// \return True if the extension was enabled, false otherwise.
    ///
    bool enableExtension(const char *requiredExtensionName);

    std::vector<const char *> enabledExtensions;
    std::vector<vk::ExtensionProperties> availableExtensions;

    vk::raii::Device handle{VK_NULL_HANDLE};

    vk::raii::SurfaceKHR &surface;
    PhysicalDevice &gpu;
    VmaAllocator vmaAllocator{VK_NULL_HANDLE};
    std::vector<std::vector<Queue>> queues;
};
} // namespace vkf::core

#endif // VULKANRENDERER_DEVICE_H