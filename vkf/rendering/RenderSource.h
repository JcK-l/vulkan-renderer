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

#pragma once

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

    virtual std::vector<vk::ImageView> getImageViews() const = 0;

    virtual uint32_t getImageCount() const = 0;

    virtual vk::Extent2D getExtent() const = 0;

    ///
    /// \brief Method to check if the render source has changed.
    ///
    /// This method returns true if the render source has changed.
    /// It toggles the changed flag to false.
    ///
    /// \return A boolean indicating whether or not the render source has changed.
    virtual bool resetChanged() = 0;

    virtual uint32_t getFrameIndex() = 0;
};

} // namespace vkf::rendering