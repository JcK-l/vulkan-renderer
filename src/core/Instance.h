/// \file
/// \brief

//
// Created by Joshua Lowe on 11/1/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_INSTANCE_H
#define VULKANRENDERER_INSTANCE_H


/// \brief Collection of core functionality
namespace vkf::core {
    class Instance {
    public:
        explicit Instance(const std::string &, const std::vector<const char *> & = {},
                          const std::vector<const char *> & = {});

        Instance(const Instance &) = delete;

        Instance(Instance &&) = delete;

        ~Instance() = default;

        Instance &operator=(const Instance &) = delete;

        Instance &operator=(Instance &&) = delete;

    private:

        /// \brief Enables extension if it is in availableExtensions
        /// \param requiredExtensionName Name of Extension
        /// \return IsExtensionEnabled
        bool enableExtension(const char *requiredExtensionName);

        /// \brief Enables layer if it is in availableLayers
        /// \param requiredLayerName Name of Layer
        /// \return IsLayerEnabled
        bool enableLayer(const char *requiredLayerName);

        /// \brief Tries to enable all required extensions
        /// \param requiredExtensions
        void validateExtensions(const std::vector<const char *> &requiredExtensions);

        /// \brief Tries to enable all required layers
        /// \param requiredLayers
        void validateLayers(const std::vector<const char *> &requiredLayers);

        std::vector<const char *> enabledExtensions;
        std::vector<vk::ExtensionProperties> availableExtensions;

        std::vector<const char *> enabledLayers;
        std::vector<vk::LayerProperties> availableLayers;

        vk::raii::Context context{};
        vk::raii::Instance handle{VK_NULL_HANDLE};
        vk::raii::DebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};

    };
} // namespace vkf::core



#endif //VULKANRENDERER_INSTANCE_H
