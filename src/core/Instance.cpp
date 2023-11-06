/// \file
/// \brief

//
// Created by Joshua Lowe on 11/1/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "../pch.h"
#include "Instance.h"
#include "../common/utils.h"
#include "../common/logging.h"

#if !defined( NDEBUG )
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             VkDebugUtilsMessengerCallbackDataEXT const *callbackData,
                                             void * /*pUserData*/ ) {
  // Log debug message
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    LOGW("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    LOGE("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
  }
  return VK_FALSE;
}

#endif

namespace vkf::core {

    Instance::Instance(const std::string &name, const std::vector<const char *> &requiredExtensions,
                       const std::vector<const char *> &requiredLayers) {
      try {
        vk::DynamicLoader dl;
        auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        availableExtensions = context.enumerateInstanceExtensionProperties();
        validateExtensions(requiredExtensions);

        availableLayers = context.enumerateInstanceLayerProperties();
        validateLayers(requiredLayers);

#if !defined( NDEBUG )
        enableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        enableLayer("VK_LAYER_KHRONOS_validation");
        enableLayer("VK_LAYER_LUNARG_monitor");
#endif

        vk::ApplicationInfo applicationInfo{
          .pApplicationName = name.c_str(),
          .applicationVersion = 1,
          .pEngineName = "engine",
          .engineVersion = 1,
          .apiVersion = VK_API_VERSION_1_3};

#if !defined( NDEBUG )
        vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> instanceCreateInfo = {
          vk::InstanceCreateInfo{
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
            .ppEnabledLayerNames = enabledLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
            .ppEnabledExtensionNames = enabledExtensions.data()},
          common::utils::createDebugMessengerInfo(&debugCallback)
        };
#else
        vk::StructureChain<vk::InstanceCreateInfo> instanceCreateInfo = {
      vk::InstanceCreateInfo{
        .pApplicationInfo = &applicationInfo},
    };
#endif
        handle = vk::raii::Instance{context, instanceCreateInfo.get<vk::InstanceCreateInfo>()};
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*handle);

#if !defined( NDEBUG )
        debugMessenger = vk::raii::DebugUtilsMessengerEXT{handle,
                                                          common::utils::createDebugMessengerInfo(&debugCallback)};
#endif

      } catch (vk::SystemError &err) {
        LOGE("vk::SystemError: {}", err.what());
        exit(-1);
      } catch (std::exception &err) {
        LOGE("std::exception: {}", err.what());
        exit(-1);
      } catch (...) {
        LOGE("unknown error");
        exit(-1);
      }
    }

    bool Instance::enableExtension(const char *requiredExtensionName) {
      for (auto &availableExtension: availableExtensions) {
        if (strcmp(availableExtension.extensionName, requiredExtensionName) == 0) {
          auto it = std::find_if(enabledExtensions.begin(), enabledExtensions.end(),
                                 [requiredExtensionName](const char *enabledExtensionName) {
                                     return strcmp(enabledExtensionName, requiredExtensionName) == 0;
                                 });
          if (it != enabledExtensions.end()) {
            // Extension is already enabled
          } else {
            LOGI("Extension {} found, enabling it", requiredExtensionName)
            enabledExtensions.emplace_back(requiredExtensionName);
          }
          return true;
        }
      }

      LOGI("Extension {} not found", requiredExtensionName)
      return false;
    }

    bool Instance::enableLayer(const char *requiredLayerName) {
      for (auto &availableLayer: availableLayers) {
        if (strcmp(availableLayer.layerName, requiredLayerName) == 0) {
          auto it = std::find_if(enabledLayers.begin(), enabledLayers.end(),
                                 [requiredLayerName](const char *enabledLayerName) {
                                     return strcmp(enabledLayerName, requiredLayerName) == 0;
                                 });
          if (it != enabledLayers.end()) {
            // Extension is already enabled
          } else {
            LOGI("Layer {} found, enabling it", requiredLayerName)
            enabledLayers.emplace_back(requiredLayerName);
          }
          return true;
        }
      }

      LOGI("Layer {} not found", requiredLayerName)
      return false;
    }

    void Instance::validateExtensions(const std::vector<const char *> &requiredExtensions) {
      for (auto extension: requiredExtensions) {
        if (!enableExtension(extension)) {
          LOGE("Required instance extension {} not available, cannot run", extension)
          throw std::runtime_error("Required instance extensions are missing.");
        }
      }
    }

    void Instance::validateLayers(const std::vector<const char *> &requiredLayers) {
      for (auto layer: requiredLayers) {
        if (!enableLayer(layer)) {
          LOGE("Required instance layer {} not available, cannot run", layer)
          throw std::runtime_error("Required instance layers are missing.");
        }
      }
    }
} // namespace vkf::core