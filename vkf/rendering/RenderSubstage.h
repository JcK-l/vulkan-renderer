////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RenderSubstage.h
/// \brief This file declares the abstract RenderSubstage class which is used for managing Vulkan rendering sub-stages.
///
/// The RenderSubstage class is part of the vkf::rendering namespace. It provides an abstract interface for interacting
/// with Vulkan rendering sub-stages. This class is intended to be subclassed by specific types of rendering substages.
/// Each subclass should provide an implementation for the draw method.
///
/// \author Joshua Lowe
/// \date 11/21/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace vkf::rendering
{

///
/// \class RenderSubstage
/// \brief This abstract class provides an interface for managing Vulkan rendering sub-stages.
///
/// This class is intended to be subclassed by specific types of rendering substages. Each subclass should provide an
/// implementation for the draw method, which is used to perform the actual rendering operation.
///
class RenderSubstage
{
  public:
    ///
    /// \brief Destructor for the RenderSubstage class.
    ///
    /// This is the default destructor. It is declared as virtual to ensure correct destruction of subclasses.
    ///
    virtual ~RenderSubstage() = default;

    ///
    /// \brief Pure virtual method to draw.
    ///
    /// This method is intended to be overridden by subclasses to perform the actual rendering operation.
    /// It takes a command buffer as an argument, which is used to record the rendering commands.
    ///
    /// \param cmd The command buffer.
    ///
    virtual void draw(vk::raii::CommandBuffer *cmd) = 0;

    virtual std::string getType() = 0;

  private:
};

} // namespace vkf::rendering