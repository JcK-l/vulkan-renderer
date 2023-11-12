/// \file
/// \brief

//
// Created by Joshua Lowe on 11/8/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_PHYSICALDEVICE_H
#define VULKANRENDERER_PHYSICALDEVICE_H

namespace vkf::core {

/// \brief This is a wrapper class for vk::raii::PhysicalDevice
    class PhysicalDevice {
    public:
        explicit PhysicalDevice(vk::raii::PhysicalDevice &physicalDevice);

        PhysicalDevice(const PhysicalDevice &) = delete;

        PhysicalDevice(PhysicalDevice &&) = delete;

        ~PhysicalDevice() = default;

        PhysicalDevice &operator=(const PhysicalDevice &) = delete;

        PhysicalDevice &operator=(PhysicalDevice &&) = delete;

        [[nodiscard]] const vk::PhysicalDeviceFeatures &getFeatures() const;

        [[nodiscard]] const vk::PhysicalDeviceProperties &getProperties() const;

        [[nodiscard]] const std::vector<vk::QueueFamilyProperties> &getQueueFamilyProperties() const;

        [[nodiscard]] bool getSurfaceSupportKHR(uint32_t queueFamilyIndex, vk::SurfaceKHR surface) const;

        [[nodiscard]] const vk::raii::PhysicalDevice &getHandle() const;

    private:

        vk::raii::PhysicalDevice handle{VK_NULL_HANDLE};

        vk::PhysicalDeviceFeatures features;
        vk::PhysicalDeviceProperties properties;
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
    };
} // namespace vkf::core


#endif //VULKANRENDERER_PHYSICALDEVICE_H
