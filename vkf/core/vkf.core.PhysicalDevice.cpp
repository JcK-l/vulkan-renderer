/// \file
/// \brief

//
// Created by Joshua Lowe on 11/13/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

module;

#include "../common/Log.h"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

module vkf.core.PhysicalDevice;

namespace vkf::core {

    PhysicalDevice::PhysicalDevice(vk::raii::PhysicalDevice &physicalDevice) : handle(physicalDevice) {
      features = physicalDevice.getFeatures();
      properties = physicalDevice.getProperties();
      queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

      LOG_INFO("Found GPU: {}", properties.deviceName.data());
    }

    const vk::PhysicalDeviceFeatures &PhysicalDevice::getFeatures() const {
      return features;
    }

    const vk::PhysicalDeviceProperties &PhysicalDevice::getProperties() const {
      return properties;
    }

    const std::vector<vk::QueueFamilyProperties> &PhysicalDevice::getQueueFamilyProperties() const {
      return queueFamilyProperties;
    }

    bool PhysicalDevice::getSurfaceSupportKHR(uint32_t queueFamilyIndex, vk::SurfaceKHR surface) const {
      return handle.getSurfaceSupportKHR(queueFamilyIndex, surface);
    }

    const vk::raii::PhysicalDevice &PhysicalDevice::getHandle() const {
      return handle;
    }
} // namespace vkf::core
