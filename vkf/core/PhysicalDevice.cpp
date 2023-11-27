////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PhysicalDevice.cpp
/// \brief This file implements the PhysicalDevice class which is used to manage Vulkan physical
/// devices.
///
/// The PhysicalDevice class is part of the vkf::core namespace. It provides methods for interacting with a Vulkan
/// physical device, including getting device features and properties, querying queue family properties, checking
/// surface support, and requesting extension features.
///
/// \author Joshua Lowe
/// \date 11/8/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysicalDevice.h"
#include "../common/Log.h"

namespace vkf::core
{
PhysicalDevice::PhysicalDevice(vk::raii::PhysicalDevice physicalDevice) : handle{std::move(physicalDevice)}
{
    physicalDeviceFeatures = handle.getFeatures();
    properties = handle.getProperties();
    queueFamilyProperties = handle.getQueueFamilyProperties();

    LOG_INFO("Found GPU: {}", properties.deviceName.data())
}

const vk::PhysicalDeviceFeatures &PhysicalDevice::getPhysicalDeviceFeatures() const
{
    return physicalDeviceFeatures;
}

const vk::PhysicalDeviceProperties &PhysicalDevice::getProperties() const
{
    return properties;
}

const std::vector<vk::QueueFamilyProperties> &PhysicalDevice::getQueueFamilyProperties() const
{
    return queueFamilyProperties;
}

bool PhysicalDevice::getSurfaceSupportKHR(uint32_t queueFamilyIndex, vk::SurfaceKHR surface) const
{
    return handle.getSurfaceSupportKHR(queueFamilyIndex, surface);
}

const vk::raii::PhysicalDevice &PhysicalDevice::getHandle() const
{
    return handle;
}

void *PhysicalDevice::getExtensionFeaturesHead() const
{
    return extensionFeaturesHead;
}

} // namespace vkf::core