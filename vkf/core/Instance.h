////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Instance.h
/// \brief This file declares the Instance class which is used for managing Vulkan instances.
///
/// The Instance class is part of the vkf::core namespace. It provides methods for creating a Vulkan instance,
/// validating extensions and layers, and querying GPUs. In debug build mode, it also creates a debug messenger.
///
/// \author Joshua Lowe
/// \date 11/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_INSTANCE_H
#define VULKANRENDERER_INSTANCE_H

namespace vkf::core
{

// Forward declarations
class PhysicalDevice;

///
/// \class Instance
/// \brief This class manages Vulkan instances.
///
/// It provides methods for managing a Vulkan instance, validating extensions and layers, and querying GPUs. In debug
/// build mode, it also creates a debug messenger.
///
class Instance
{
  public:
    ///
    /// \brief Constructs an Instance object.
    ///
    /// This constructor creates a Vulkan instance using the provided application name, required extensions, and
    /// required layers.
    ///
    /// \param appName The name of the application.
    /// \param requiredExtensions A vector of required extensions (optional).
    /// \param requiredLayers A vector of required layers (optional).
    ///
    explicit Instance(const std::string &appName, const std::vector<const char *> &requiredExtensions = {},
                      const std::vector<const char *> &requiredLayers = {});

    Instance(const Instance &) = delete;            ///< Deleted copy constructor
    Instance(Instance &&) noexcept = default;       ///< Default move constructor
    Instance &operator=(const Instance &) = delete; ///< Deleted copy assignment operator
    Instance &operator=(Instance &&) = delete;      ///< Deleted move assignment operator
    ~Instance(); ///< Implementation in Instance.cpp because of std::unique_ptr forward declaration

    ///
    /// \brief Retrieves the first suitable GPU.
    ///
    /// This function returns the first suitable GPU for the provided surface.
    /// If no suitable GPU is found the function returns the first GPU.
    ///
    /// \param surface The surface for which to find a suitable GPU.
    /// \return A reference to the first suitable GPU.
    PhysicalDevice &getSuitableGpu(vk::raii::SurfaceKHR &surface);

    [[nodiscard]] const vk::raii::Instance &getHandle() const;

    [[nodiscard]] const vk::raii::Context &getContext() const;

  private:
    ///
    /// \brief Validates the required extensions.
    ///
    /// This function throws a runtime_error if a required extension is not available.
    ///
    /// \param requiredExtensions A vector of required extensions.
    ///
    void validateExtensions(const std::vector<const char *> &requiredExtensions);

    ///
    /// \brief Validates the required layers.
    ///
    /// This function throws a runtime_error if a required layer is not available.
    ///
    /// \param requiredLayers A vector of required layers.
    ///
    void validateLayers(const std::vector<const char *> &requiredLayers);

    ///
    /// \brief Enables an extension.
    ///
    /// \param requiredExtensionName The name of the extension to enable.
    /// \return True if the extension was enabled, false otherwise.
    ///
    bool enableExtension(const char *requiredExtensionName);

    ///
    /// \brief Enables a layer.
    ///
    /// \param requiredLayerName The name of the layer to enable.
    /// \return True if the layer was enabled, false otherwise.
    ///
    bool enableLayer(const char *requiredLayerName);

    ///
    /// \brief Queries the available GPUs.
    ///
    /// This function creates a PhysicalDevice object for each available GPU.
    ///
    void queryGpus();

    std::vector<const char *> enabledExtensions;
    std::vector<vk::ExtensionProperties> availableExtensions;

    std::vector<const char *> enabledLayers;
    std::vector<vk::LayerProperties> availableLayers;

    vk::raii::Context context{};
    vk::raii::Instance handle{VK_NULL_HANDLE};
    vk::raii::DebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};
    std::vector<std::unique_ptr<PhysicalDevice>> gpus;
};
} // namespace vkf::core

#endif // VULKANRENDERER_INSTANCE_H