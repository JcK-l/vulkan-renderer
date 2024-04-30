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

#pragma once

#include "vk_mem_alloc.h"

// Forward declarations
#include "CoreFwd.h"

namespace vkf::core
{

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
    Image(const Device &device, vk::ImageCreateInfo createInfo, VmaAllocationCreateFlags allocationFlags);

    Image(const Image &) = delete;            ///< Deleted copy constructor
    Image(Image &&) noexcept;                 ///< Default move constructor
    Image &operator=(const Image &) = delete; ///< Deleted copy assignment operator
    Image &operator=(Image &&) = delete;      ///< Deleted move assignment operator
    ~Image();                                 ///< Destructor

    [[nodiscard]] vk::ImageView getImageView(vk::ImageAspectFlags aspectFlags);
    [[nodiscard]] vk::Sampler getSampler();

    ///
    /// \brief Map memory to the image.
    ///
    /// \param data A pointer to the data to be mapped.
    ///
    void mapMemory();

    ///
    /// \brief Unmap memory from the image.
    ///
    void unmapMemory();

    ///
    /// \brief Transition the image layout.
    ///
    /// \param newLayout The new layout for the image.
    ///
    void transitionImageLayout(vk::ImageLayout newLayout);

    ///
    /// \brief Copies the contents of the srcBuffer to this image.
    ///
    /// \param srcBuffer The buffer to copy from. Inteded to be a staging buffer.
    ///
    void copyBufferToImage(const Buffer &srcBuffer);

  private:
    void createImageView(vk::ImageAspectFlags aspectFlags);
    void createSampler();

    const Device &device;

    bool mapped{false};
    void *mappedData{nullptr};

    vk::ImageCreateInfo createInfo;
    vk::ImageLayout currentLayout{vk::ImageLayout::eUndefined};

    VmaAllocation allocation{VK_NULL_HANDLE};
    VkImage handle;
    vk::raii::ImageView imageView = VK_NULL_HANDLE;
    vk::ImageAspectFlags currentAspectFlags{};
    vk::raii::Sampler sampler = VK_NULL_HANDLE;
};

} // namespace vkf::core