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

#ifndef VULKANRENDERER_UTILITY_H
#define VULKANRENDERER_UTILITY_H

namespace vkf::scene // Forward declarations
{
enum class PrefabType;
} // namespace vkf::scene

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

///
/// \brief Converts a PrefabType to a string.
///
/// \param type The PrefabType to convert to a string.
/// \return  A string representation of the PrefabType.
///
std::string getPrefabTypeString(scene::PrefabType type);

} // namespace vkf

#endif // VULKANRENDERER_UTILITY_H