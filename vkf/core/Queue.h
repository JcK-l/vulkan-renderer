////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Queue.h
/// \brief This file declares the Queue class which is used for managing Vulkan queues.
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

#ifndef VULKANRENDERER_QUEUE_H
#define VULKANRENDERER_QUEUE_H

namespace vkf::core
{

///
/// \class Queue
/// \brief This class manages Vulkan queues.
///
/// It provides an interface for interacting with a Vulkan queue, including getting the handle to the queue, the family
/// index, the queue properties, and the queue index. It also provides a method to check if the queue can present.
///
class Queue
{
  public:
    ///
    /// \brief Constructs a Queue object.
    ///
    /// This constructor creates a Vulkan queue using the provided queue, family index, queue properties, present
    /// support, and queue index.
    ///
    /// \param queue The Vulkan queue.
    /// \param familyIndex The family index of the queue.
    /// \param properties The properties of the queue.
    /// \param canPresent A boolean indicating if the queue can present.
    /// \param queueIndex The index of the queue.
    ///
    Queue(vk::raii::Queue queue, uint32_t familyIndex, vk::QueueFamilyProperties properties, vk::Bool32 canPresent,
          uint32_t queueIndex);

    Queue(const Queue &) = delete;            ///< Deleted copy constructor
    Queue(Queue &&) noexcept = default;       ///< Default move constructor
    Queue &operator=(const Queue &) = delete; ///< Deleted copy assignment operator
    Queue &operator=(Queue &&) = delete;      ///< Deleted move assignment operator
    ~Queue() = default;                       ///< Default destructor

    [[nodiscard]] const vk::raii::Queue &getHandle() const;

    [[nodiscard]] uint32_t getFamilyIndex() const;

    [[nodiscard]] const vk::QueueFamilyProperties &getProperties() const;

    [[nodiscard]] vk::Bool32 canPresent() const;

    [[nodiscard]] uint32_t getQueueIndex() const;

  private:
    vk::raii::Queue handle{VK_NULL_HANDLE};
    uint32_t familyIndex;
    vk::QueueFamilyProperties properties;
    vk::Bool32 supportsPresent;
    uint32_t queueIndex;
};
} // namespace vkf::core

#endif // VULKANRENDERER_QUEUE_H