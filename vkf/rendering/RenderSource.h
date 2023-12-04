////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RenderSource.h
/// \brief This file declares the abstract RenderSource class which is used for managing Vulkan rendering operations.
///
/// The RenderSource class is part of the vkf::rendering namespace. It provides an abstract interface for interacting
/// with Vulkan rendering operations. This class is intended to be subclassed by specific types of rendering sources.
/// Each subclass should provide an implementation for the getImageViews, getImageCount, and getExtent methods.
///
/// \author Joshua Lowe
/// \date 12/3/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_RENDERSOURCE_H
#define VULKANRENDERER_RENDERSOURCE_H

namespace vkf::rendering
{

///
/// \class RenderSource
/// \brief This abstract class provides an interface for managing Vulkan rendering operations.
///
/// This class is intended to be subclassed by specific types of rendering sources. Each subclass should provide an
/// implementation for the getImageViews, getImageCount, and getExtent methods.
///
class RenderSource
{
  public:
    ///
    /// \brief Destructor for the RenderSource class.
    ///
    /// This is the default destructor. It is declared as virtual to ensure correct destruction of subclasses.
    ///
    virtual ~RenderSource() = default;

    ///
    /// \brief Pure virtual method to get the image views.
    ///
    /// This method is intended to be overridden by subclasses to return a vector of image views.
    ///
    /// \return A vector of image views.
    ///
    virtual std::vector<vk::ImageView> getImageViews() const = 0;

    ///
    /// \brief Pure virtual method to get the image count.
    ///
    /// This method is intended to be overridden by subclasses to return the image count.
    ///
    /// \return The image count.
    ///
    virtual uint32_t getImageCount() const = 0;

    ///
    /// \brief Pure virtual method to get the extent.
    ///
    /// This method is intended to be overridden by subclasses to return the extent.
    ///
    /// \return The extent.
    ///
    virtual vk::Extent2D getExtent() const = 0;
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_RENDERSOURCE_H