/// \file
/// \brief

//
// Created by Joshua Lowe on 11/1/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_UTILS_H
#define VULKANRENDERER_UTILS_H

#include "../pch.h"

/// \brief Collection of utility functions
namespace vkf::common::utils {
    /// \brief Creates a debug create info
    /// \param func Debug callback function
    /// \return Create info
    vk::DebugUtilsMessengerCreateInfoEXT
    createDebugMessengerInfo(VKAPI_ATTR VkBool32 VKAPI_CALL (*func)(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                                    VkDebugUtilsMessageTypeFlagsEXT,
                                                                    VkDebugUtilsMessengerCallbackDataEXT const *,
                                                                    void * /*pUserData*/));
} // namespace vkf::utils

#endif //VULKANRENDERER_UTILS_H
