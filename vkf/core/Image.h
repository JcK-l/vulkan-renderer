////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Image.h
/// \brief This file declares the Image class which is used for managing Vulkan images.
///
/// The Image class is part of the vkf::core namespace. It provides functionality to get the handle to the image,
/// map memory to the image, and unmap memory from the image.
///
/// \author Joshua Lowe
/// \date 12/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_IMAGE_H
#define VULKANRENDERER_IMAGE_H

#include "vk_mem_alloc.h"

namespace vkf::core
{

// Forward declarations
class Queue;
class Device;

///
/// \class Image
/// \brief This class manages Vulkan images.
///
/// It provides functionality to get the handle to the image, map memory to the image, and unmap memory from the image.
///
class Image
{
  public:
    ///
    /// \brief Constructor for the Image class.
    ///
    /// \param device The Vulkan device.
    /// \param createInfo The information for creating an image.
    /// \param allocationFlags The flags for memory allocation.
    ///
    Image(const Device &device, vk::ImageCreateInfo createInfo, VmaAllocationCreateFlagBits allocationFlags);

    Image(const Image &) = delete;            // Deleted copy constructor
    Image(Image &&) noexcept = default;       // Default move constructor
    Image &operator=(const Image &) = delete; // Deleted copy assignment operator
    Image &operator=(Image &&) = delete;      // Deleted move assignment operator
    ~Image();                                 // Destructor

    [[nodiscard]] vk::raii::ImageView createImageView() const;

    ///
    /// \brief Map memory to the image.
    ///
    /// \param data A pointer to the data to be mapped.
    ///
    void mapMemory(void **data);

    ///
    /// \brief Unmap memory from the image.
    ///
    void unmapMemory();

  private:
    const Device &device;

    vk::ImageCreateInfo createInfo;

    VmaAllocation allocation{VK_NULL_HANDLE};
    VkImage handle;
};

} // namespace vkf::core

#endif // VULKANRENDERER_IMAGE_H