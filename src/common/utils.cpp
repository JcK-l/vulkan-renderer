/// \file
/// \brief

//
// Created by Joshua Lowe on 11/1/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "utils.h"

namespace vkf::common::utils {
    vk::DebugUtilsMessengerCreateInfoEXT
    createDebugMessengerInfo(VKAPI_ATTR VkBool32 VKAPI_CALL (*func)(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                                    VkDebugUtilsMessageTypeFlagsEXT,
                                                                    VkDebugUtilsMessengerCallbackDataEXT const *,
                                                                    void * /*pUserData*/)) {
      return vk::DebugUtilsMessengerCreateInfoEXT{
        .messageSeverity =
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType =
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        .pfnUserCallback = func,
      };
    }
} // namespace vkf::utils
