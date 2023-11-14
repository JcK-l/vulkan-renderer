/// \file
/// \brief

//
// Created by Joshua Lowe on 11/1/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "../pch.h"
#include "Instance.h"
//#include "../common/utils.h"
#include "../common/Log.h"

#if !defined( NDEBUG )
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             VkDebugUtilsMessengerCallbackDataEXT const *callbackData,
                                             void * /*pUserData*/ ) {
  // Log debug message
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    LOG_WARN("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    LOG_ERROR("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);
  }
  return VK_FALSE;
}

#endif

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vkf::core {

    Instance::Instance(const std::string &appName, const std::vector<const char *> &requiredExtensions,
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
          .pApplicationName = appName.c_str(),
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
          vk::DebugUtilsMessengerCreateInfoEXT{
            .messageSeverity =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType =
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            .pfnUserCallback = &debugCallback,
          }
        };
#else
        vk::StructureChain<vk::InstanceCreateInfo> instanceCreateInfo = {
          vk::InstanceCreateInfo{
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
            .ppEnabledLayerNames = enabledLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
            .ppEnabledExtensionNames = enabledExtensions.data()}
        };
#endif
        handle = vk::raii::Instance{context, instanceCreateInfo.get<vk::InstanceCreateInfo>()};
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*handle);
        LOG_INFO("Created instance");

#if !defined( NDEBUG )
        debugMessenger = vk::raii::DebugUtilsMessengerEXT{handle,
                                                          vk::DebugUtilsMessengerCreateInfoEXT{
                                                            .messageSeverity =
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                                                            .messageType =
                                                            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                                            .pfnUserCallback = &debugCallback,
                                                          }};
        LOG_DEBUG("Created debug messenger");
#endif

        queryGpus();

      } catch (vk::SystemError &err) {
        LOG_ERROR("vk::SystemError: {}", err.what());
        exit(-1);
      } catch (std::exception &err) {
        LOG_ERROR("std::exception: {}", err.what());
        exit(-1);
      } catch (...) {
        LOG_ERROR("unknown error");
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
            LOG_INFO("Instance extension {} is already enabled", requiredExtensionName)
          } else {
            LOG_INFO("Enabling instance extension: {}", requiredExtensionName)
            enabledExtensions.emplace_back(requiredExtensionName);
          }
          return true;
        }
      }

      LOG_INFO("Instance extension {} not found", requiredExtensionName)
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
            LOG_INFO("Instance layer {} is already enabled", requiredLayerName)
          } else {
            LOG_INFO("Enabling instance layer: {}", requiredLayerName)
            enabledLayers.emplace_back(requiredLayerName);
          }
          return true;
        }
      }

      LOG_INFO("Instance layer {} not found", requiredLayerName)
      return false;
    }

    void Instance::validateExtensions(const std::vector<const char *> &requiredExtensions) {
      for (auto extension: requiredExtensions) {
        if (!enableExtension(extension)) {
          LOG_ERROR("Required instance extension {} is not available", extension)
          throw std::runtime_error("Required instance extensions are missing.");
        }
      }
    }

    void Instance::validateLayers(const std::vector<const char *> &requiredLayers) {
      for (auto layer: requiredLayers) {
        if (!enableLayer(layer)) {
          LOG_ERROR("Required instance layer {} is not available", layer)
          throw std::runtime_error("Required instance layers are missing.");
        }
      }
    }

    void Instance::queryGpus() {
      // Querying valid physical devices on the machine
      vk::raii::PhysicalDevices physicalDevices{handle};
      if (physicalDevices.empty()) {
        throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
      }

      // Create gpus wrapper objects from the vk::PhysicalDevice's
      for (auto &physicalDevice: physicalDevices) {
        gpus.push_back(std::make_unique<PhysicalDevice>(physicalDevice));
      }
    }

    PhysicalDevice &Instance::getSuitableGpu(vk::raii::SurfaceKHR &surface) {
      assert(!gpus.empty() && "No physical devices found");

      for (const auto &gpu: gpus) {
        auto queueFamilyProperties = gpu->getQueueFamilyProperties();
        auto size = queueFamilyProperties.size();

        // check for discrete gpu
        if (gpu->getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {

          // check if a queue can present
          for (auto i = 0; i < size; ++i) {
            if (gpu->getSurfaceSupportKHR(i, *surface)) {
              return *gpu;
            }
          }
        }
      }
      LOG_WARN("No suitable gpu found. Picking first GPU: {}", gpus[0]->getProperties().deviceName.data());
      return *gpus[0];
    }

    const vk::raii::Instance &Instance::getHandle() const {
      return handle;
    }

    const vk::raii::Context &Instance::getContext() const {
      return context;
    }
} // namespace vkf::core