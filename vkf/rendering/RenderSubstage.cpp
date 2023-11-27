////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RenderSubstage.cpp
/// \brief This file implements the RenderSubstage class which is used for managing Vulkan rendering substages.
///
/// The RenderSubstage class is part of the vkf::rendering namespace. It provides an interface for interacting with
/// Vulkan rendering sub-stages, including a method for drawing using a provided command buffer.
///
/// \author Joshua Lowe
/// \date 11/21/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RenderSubstage.h"

namespace vkf::rendering
{

RenderSubstage::RenderSubstage()
{
}

void RenderSubstage::draw(vk::CommandBuffer *cmd)
{
}

} // namespace vkf::rendering