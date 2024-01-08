////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Buffer.h
/// \brief This file declares the Buffer class which is used for managing Vulkan buffers.
///
/// The Buffer class is part of the vkf::core namespace. It provides functionality to get the handle to the buffer,
/// map memory to the buffer, and unmap memory from the buffer.
///
/// \author Joshua Lowe
/// \date 12/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_BUFFER_H
#define VULKANRENDERER_BUFFER_H

#include "vk_mem_alloc.h"

namespace vkf::core
{

// Forward declarations
class Queue;
class Device;

///
/// \class Buffer
/// \brief This class manages Vulkan buffers.
///
/// It provides functionality to get the handle to the buffer, map memory to the buffer, and unmap memory from the
/// buffer.
///
class Buffer
{
  public:
    ///
    /// \brief Constructor for the Buffer class.
    ///
    /// \param device The Vulkan device.
    /// \param createInfo The information for creating a buffer.
    /// \param memoryUsage The intended usage of the memory.
    ///
    Buffer(const Device &device, vk::BufferCreateInfo createInfo, VmaAllocationCreateFlags allocationFlags);

    Buffer(const Buffer &) = delete;            ///< Deleted copy constructor
    Buffer(Buffer &&) noexcept;                 ///< Move constructor
    Buffer &operator=(const Buffer &) = delete; ///< Deleted copy assignment operator
    Buffer &operator=(Buffer &&) = delete;      ///< Deleted move assignment operator
    ~Buffer();                                  ///< Destructor

    ///
    /// \brief Map memory to the buffer.
    ///
    /// \param data A pointer to the data to be mapped.
    ///
    void mapMemory();

    ///
    /// \brief Unmap memory from the buffer.
    ///
    void unmapMemory();

    void updateData(const void *data, const uint32_t size, const uint32_t offset);

    ///
    /// \brief Copies the contents of the srcBuffer to this buffer.
    ///
    /// This method copies the contents of the srcBuffer to this buffer. The srcBuffer must be the same size as this
    /// buffer.
    ///
    /// \param srcBuffer The buffer to copy from. Inteded to be a staging buffer.
    ///
    void copyBuffer(const Buffer &srcBuffer);

    [[nodiscard]] vk::Buffer getBuffer() const;
    [[nodiscard]] uint32_t getSize() const;

  private:
    const Device &device;

    bool persistentMapped{false};
    bool mapped{false};
    void *mappedData{nullptr};

    uint32_t size{0};

    VmaAllocation allocation;
    VkBuffer handle;
};

} // namespace vkf::core

#endif // VULKANRENDERER_BUFFER_H