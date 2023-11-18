/// \file
/// \brief

//
// Created by Joshua Lowe on 11/8/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_PHYSICALDEVICE_H
#define VULKANRENDERER_PHYSICALDEVICE_H

#include "../common/Log.h"

namespace vkf::core
{

/// \brief This is a wrapper class for vk::raii::PhysicalDevice
class PhysicalDevice
{
  public:
    explicit PhysicalDevice(vk::raii::PhysicalDevice &physicalDevice);

    PhysicalDevice(const PhysicalDevice &) = delete;

    PhysicalDevice(PhysicalDevice &&) = delete;

    ~PhysicalDevice() = default;

    PhysicalDevice &operator=(const PhysicalDevice &) = delete;

    PhysicalDevice &operator=(PhysicalDevice &&) = delete;

    [[nodiscard]] const vk::PhysicalDeviceFeatures &getPhysicalDeviceFeatures() const;

    [[nodiscard]] const vk::PhysicalDeviceProperties &getProperties() const;

    [[nodiscard]] const std::vector<vk::QueueFamilyProperties> &getQueueFamilyProperties() const;

    [[nodiscard]] bool getSurfaceSupportKHR(uint32_t queueFamilyIndex, vk::SurfaceKHR surface) const;

    [[nodiscard]] const vk::raii::PhysicalDevice &getHandle() const;

    [[nodiscard]] void *getExtensionFeaturesHead() const;

    template <typename Structure> Structure &requestExtensionFeatures()
    {

        // Check if the extension feature has already been requested
        vk::StructureType structureType = Structure::structureType;
        auto featuresIt = enabledExtensionFeatures.find(structureType);
        if (featuresIt != enabledExtensionFeatures.end())
        {
            return *static_cast<Structure *>(featuresIt->second.get());
        }

        // Create a new feature chain with the extension feature
        vk::StructureChain<vk::PhysicalDeviceFeatures2KHR, Structure> featureChain =
            handle.getFeatures2KHR<vk::PhysicalDeviceFeatures2KHR, Structure>();

        enabledExtensionFeatures.insert(
            {structureType, std::make_shared<Structure>(featureChain.template get<Structure>())});

        // Get the pointer to the extension feature
        auto *featurePointer = static_cast<Structure *>(enabledExtensionFeatures.find(structureType)->second.get());

        // Make sure the feature is added to the head of the feature chain
        if (extensionFeaturesHead)
        {
            featurePointer->pNext = extensionFeaturesHead;
        }
        extensionFeaturesHead = featurePointer;
        LOG_INFO("Enabled extension feature: {}", vk::to_string(structureType));

        return *featurePointer;
    }

  private:
    vk::raii::PhysicalDevice handle{VK_NULL_HANDLE};

    std::map<vk::StructureType, std::shared_ptr<void>> enabledExtensionFeatures;
    void *extensionFeaturesHead{nullptr};

    vk::PhysicalDeviceFeatures physicalDeviceFeatures;
    vk::PhysicalDeviceProperties properties;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
};
} // namespace vkf::core

#endif // VULKANRENDERER_PHYSICALDEVICE_H
