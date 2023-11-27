////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PhysicalDevice.h
/// \brief This file declares the PhysicalDevice class which is used for managing Vulkan physical devices.
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

#ifndef VULKANRENDERER_PHYSICALDEVICE_H
#define VULKANRENDERER_PHYSICALDEVICE_H

namespace vkf::core
{

///
/// \class PhysicalDevice
/// \brief This class manages Vulkan physical devices.
///
/// It provides methods for interacting with a Vulkan physical device, including getting device features and properties,
/// querying queue family properties, checking surface support, and requesting extension features.
///
class PhysicalDevice
{
  public:
    ///
    /// \brief Constructor for the PhysicalDevice class.
    ///
    /// This constructor creates a Vulkan physical device using the provided physical device.
    ///
    /// \param physicalDevice The Vulkan physical device.
    ///
    explicit PhysicalDevice(vk::raii::PhysicalDevice physicalDevice);

    PhysicalDevice(const PhysicalDevice &) = delete;            // Deleted copy constructor
    PhysicalDevice(PhysicalDevice &&) noexcept = default;       // Default move constructor
    PhysicalDevice &operator=(const PhysicalDevice &) = delete; // Deleted copy assignment operator
    PhysicalDevice &operator=(PhysicalDevice &&) = delete;      // Deleted move assignment operator
    ~PhysicalDevice() = default;                                // Default destructor

    [[nodiscard]] const vk::PhysicalDeviceFeatures &getPhysicalDeviceFeatures() const;

    [[nodiscard]] const vk::PhysicalDeviceProperties &getProperties() const;

    [[nodiscard]] const std::vector<vk::QueueFamilyProperties> &getQueueFamilyProperties() const;

    [[nodiscard]] bool getSurfaceSupportKHR(uint32_t queueFamilyIndex, vk::SurfaceKHR surface) const;

    [[nodiscard]] const vk::raii::PhysicalDevice &getHandle() const;

    ///
    /// \brief Get the head of the extension features chain.
    ///
    /// \return A pointer to the head of the extension features.
    ///
    [[nodiscard]] void *getExtensionFeaturesHead() const;

    ///
    /// \brief Request extension features.
    ///
    /// This function requests extension features for the physical device and stores them in a map for later reference.
    ///
    /// \tparam Structure The structure of the extension feature.
    /// \return A reference to the requested extension feature.
    ///
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
        // I don't want to include Log.h here, so I'll just comment this out for now
        // LOG_INFO("Enabled extension feature: {}", vk::to_string(structureType));

        return *featurePointer;
    }

  private:
    vk::raii::PhysicalDevice handle{VK_NULL_HANDLE};

    std::unordered_map<vk::StructureType, std::shared_ptr<void>> enabledExtensionFeatures;
    void *extensionFeaturesHead{nullptr};

    vk::PhysicalDeviceFeatures physicalDeviceFeatures;
    vk::PhysicalDeviceProperties properties;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
};
} // namespace vkf::core

#endif // VULKANRENDERER_PHYSICALDEVICE_H