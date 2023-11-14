/// \file
/// \brief

//
// Created by Joshua Lowe on 11/9/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Device.h"
#include "PhysicalDevice.h"
#include "../common/Log.h"

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1

#include <vk_mem_alloc.h>


namespace vkf::core {
    Device::Device(Instance &instance, vk::raii::SurfaceKHR &surface,
                   const std::vector<const char *> &requiredExtensions) {

      auto &gpu = instance.getSuitableGpu(surface);
      LOG_INFO("Picked GPU: {}", gpu.getProperties().deviceName.data());

      availableExtensions = gpu.getHandle().enumerateDeviceExtensionProperties();
      validateExtensions(requiredExtensions);

      enableExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

      std::vector<vk::QueueFamilyProperties> queueFamilyProperties = gpu.getQueueFamilyProperties();
      std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(queueFamilyProperties.size());
      std::vector<std::vector<float>> queuePriorities(queueFamilyProperties.size());

      queues.resize(queueFamilyProperties.size());

      for (uint32_t familyIndex = 0; familyIndex < queueFamilyProperties.size(); ++familyIndex) {
        const vk::QueueFamilyProperties &queueFamilyProperty = queueFamilyProperties[familyIndex];

        queuePriorities[familyIndex].resize(queueFamilyProperty.queueCount, 0.5f);

        vk::DeviceQueueCreateInfo &queueCreateInfo = queueCreateInfos[familyIndex];

        queueCreateInfo.queueFamilyIndex = familyIndex;
        queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
        queueCreateInfo.pQueuePriorities = queuePriorities[familyIndex].data();

        vk::Bool32 presentSupported = gpu.getHandle().getSurfaceSupportKHR(familyIndex, *surface);

        LOG_INFO("Found queue family with index: {} and {} queues", familyIndex, queueFamilyProperty.queueCount);
        for (uint32_t queueIndex = 0; queueIndex < queueFamilyProperty.queueCount; ++queueIndex) {
          queues[familyIndex].emplace_back(familyIndex, queueFamilyProperty, presentSupported,
                                           queueIndex);
        }
      }

      vk::DeviceCreateInfo createInfo{
        .queueCreateInfoCount=static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos=queueCreateInfos.data(),
        .enabledExtensionCount=static_cast<uint32_t>(enabledExtensions.size()),
        .ppEnabledExtensionNames=enabledExtensions.data(),
      };

      handle = vk::raii::Device{gpu.getHandle(), createInfo};
      VULKAN_HPP_DEFAULT_DISPATCHER.init(*handle);
      LOG_INFO("Created logical device");

      createVmaAllocator(instance, gpu);
    }

    void Device::validateExtensions(const std::vector<const char *> &requiredExtensions) {
      for (auto extension: requiredExtensions) {
        if (!enableExtension(extension)) {
          LOG_ERROR("Required device extension {} is not available", extension)
          throw std::runtime_error("Required device extensions are missing.");
        }
      }
    }

    bool Device::enableExtension(const char *requiredExtensionName) {
      for (auto &availableExtension: availableExtensions) {
        if (strcmp(availableExtension.extensionName, requiredExtensionName) == 0) {
          auto it = std::find_if(enabledExtensions.begin(), enabledExtensions.end(),
                                 [requiredExtensionName](const char *enabledExtensionName) {
                                     return strcmp(enabledExtensionName, requiredExtensionName) == 0;
                                 });
          if (it != enabledExtensions.end()) {
            LOG_INFO("Device extension {} is already enabled", requiredExtensionName)
          } else {
            LOG_INFO("Enabling device extension: {}", requiredExtensionName)
            enabledExtensions.emplace_back(requiredExtensionName);
          }
          return true;
        }
      }

      LOG_INFO("Device extension {} not found", requiredExtensionName)
      return false;
    }

    void Device::createVmaAllocator(const Instance &instance, const PhysicalDevice &gpu) {
      // Get pointers to required Vulkan methods
      VmaVulkanFunctions vulkanFunctions{};
      vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
      vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
      vulkanFunctions.vkAllocateMemory = vkAllocateMemory;
      vulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
      vulkanFunctions.vkBindImageMemory = vkBindImageMemory;
      vulkanFunctions.vkCreateBuffer = vkCreateBuffer;
      vulkanFunctions.vkCreateImage = vkCreateImage;
      vulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
      vulkanFunctions.vkDestroyImage = vkDestroyImage;
      vulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
      vulkanFunctions.vkFreeMemory = vkFreeMemory;
      vulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
      vulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
      vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
      vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
      vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
      vulkanFunctions.vkMapMemory = vkMapMemory;
      vulkanFunctions.vkUnmapMemory = vkUnmapMemory;
      vulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;

      // Set up VMA creation info
      VmaAllocatorCreateInfo vmaCreteInfo{};
      vmaCreteInfo.instance = *instance.getHandle();
      vmaCreteInfo.physicalDevice = *gpu.getHandle();
      vmaCreteInfo.device = *handle;
      vmaCreteInfo.vulkanApiVersion = VK_API_VERSION_1_3;
      vmaCreteInfo.pVulkanFunctions = &vulkanFunctions;
      vmaCreteInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;

      // Create VMA allocator
      vmaCreateAllocator(&vmaCreteInfo, &vmaAllocator);
      LOG_INFO("Created VMA allocator");
    }
} // namespace vkf::core