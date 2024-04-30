////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Utility.h
/// \brief This file declares utility functions for the Vulkan Framework (vkf).
///
/// The functions in this file are part of the vkf namespace. They provide utility operations
/// that are used throughout the Vulkan Framework.
///
/// \author Joshua Lowe
/// \date 11/16/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Forward declarations
#include "glm/fwd.hpp"
namespace Met3D
{
struct PointF;
}

namespace vkf
{

///
/// \brief Converts Vulkan queue flags to a string.
///
/// This function takes a vk::QueueFlags value and returns a string representation of it. This can be useful for logging
/// or debugging.
///
/// \param flags The Vulkan queue flags to convert to a string.
/// \return A string representation of the Vulkan queue flags.
///
std::string getQueueFlagsString(vk::QueueFlags flags);

glm::mat4 recompose(glm::vec3 translate, glm::quat rotation, glm::vec3 scale);

glm::vec3 calculateSrgbColor(glm::vec3 linearColor);

glm::vec3 calculateLinearColor(glm::vec3 srgbColor);

std::vector<float> createRange(float begin, float end, float step);

} // namespace vkf

namespace Met3D
{

void translate(std::vector<Met3D::PointF> &polygon, float dx, float dy);

} // namespace Met3D