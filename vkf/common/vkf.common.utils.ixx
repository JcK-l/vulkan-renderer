/// \file
/// \brief

//
// Created by Joshua Lowe on 11/13/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include <vulkan/vulkan.hpp>

export module vkf.common.utils;

///// \brief Collection of utility functions
//namespace vkf::common::utils {
//    /// \brief Creates a debug create info
//    /// \param func Debug callback function
//    /// \return Create info
//    vk::DebugUtilsMessengerCreateInfoEXT
//    createDebugMessengerInfo(VKAPI_ATTR VkBool32 VKAPI_CALL (*func)(VkDebugUtilsMessageSeverityFlagBitsEXT,
//                                                                    VkDebugUtilsMessageTypeFlagsEXT,
//                                                                    VkDebugUtilsMessengerCallbackDataEXT const *,
//                                                                    void * /*pUserData*/));
//} // namespace vkf::common::utils

//@formatter:off
module : private;
//@formatter:on

//namespace vkf::common::utils {
//    vk::DebugUtilsMessengerCreateInfoEXT
//    createDebugMessengerInfo(VKAPI_ATTR VkBool32 VKAPI_CALL (*func)(VkDebugUtilsMessageSeverityFlagBitsEXT,
//                                                                    VkDebugUtilsMessageTypeFlagsEXT,
//                                                                    VkDebugUtilsMessengerCallbackDataEXT const *,
//                                                                    void * /*pUserData*/)) {
//      return vk::DebugUtilsMessengerCreateInfoEXT{
//        .messageSeverity =
//        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
//        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
//        .messageType =
//        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
//        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
//        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
//        .pfnUserCallback = func,
//      };
//    }
//} // namespace vkf::common::utils
