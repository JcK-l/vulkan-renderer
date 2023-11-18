/// \file
/// \brief

//
// Created by Joshua Lowe on 11/9/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Device.h"
#include "../common/Log.h"
#include "PhysicalDevice.h"

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1

#include "../common/Utility.h"
#include <vk_mem_alloc.h>

namespace vkf::core
{
Device::Device(Instance &instance, vk::raii::SurfaceKHR &surface, const std::vector<const char *> &requiredExtensions)
    : surface{surface}, gpu{instance.getSuitableGpu(surface)}
{
    try
    {
        LOG_INFO("Picked GPU: {}", gpu.getProperties().deviceName.data());

        availableExtensions = gpu.getHandle().enumerateDeviceExtensionProperties();
        validateExtensions(requiredExtensions);

        auto queueCreateInfos = createQueues();
        vk::DeviceCreateInfo createInfo{.pNext = gpu.getExtensionFeaturesHead(),
                                        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                                        .pQueueCreateInfos = queueCreateInfos.data(),
                                        .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
                                        .ppEnabledExtensionNames = enabledExtensions.data(),
                                        .pEnabledFeatures = &gpu.getPhysicalDeviceFeatures()};

        handle = vk::raii::Device{gpu.getHandle(), createInfo};
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*handle);
        LOG_INFO("Created logical device");

        createVmaAllocator(instance, gpu);
    }
    catch (vk::SystemError &err)
    {
        LOG_ERROR("vk::SystemError: {}", err.what());
        exit(-1);
    }
    catch (std::exception &err)
    {
        LOG_ERROR("std::exception: {}", err.what());
        exit(-1);
    }
    catch (...)
    {
        LOG_ERROR("unknown error");
        exit(-1);
    }
}

void Device::validateExtensions(const std::vector<const char *> &requiredExtensions)
{
    for (auto extension : requiredExtensions)
    {
        if (!enableExtension(extension))
        {
            LOG_ERROR("Required device extension {} is not available", extension)
            throw std::runtime_error("Required device extensions are missing.");
        }
    }
}

bool Device::enableExtension(const char *requiredExtensionName)
{
    auto availableExtensionIt =
        std::find_if(availableExtensions.begin(), availableExtensions.end(),
                     [requiredExtensionName](const auto &availableExtension) {
                         return strcmp(availableExtension.extensionName, requiredExtensionName) == 0;
                     });

    if (availableExtensionIt != availableExtensions.end())
    {
        auto enabledExtensionIt = std::find_if(enabledExtensions.begin(), enabledExtensions.end(),
                                               [requiredExtensionName](const char *enabledExtensionName) {
                                                   return strcmp(enabledExtensionName, requiredExtensionName) == 0;
                                               });

        if (enabledExtensionIt != enabledExtensions.end())
        {
            LOG_WARN("Device extension {} is already enabled", requiredExtensionName)
        }
        else
        {
            LOG_INFO("Enabling device extension: {}", requiredExtensionName)
            enabledExtensions.emplace_back(requiredExtensionName);
        }
        return true;
    }

    LOG_WARN("Device extension {} not found", requiredExtensionName)
    return false;
}

std::vector<vk::DeviceQueueCreateInfo> Device::createQueues()
{
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = gpu.getQueueFamilyProperties();
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(queueFamilyProperties.size());
    std::vector<std::vector<float>> queuePriorities(queueFamilyProperties.size());

    queues.resize(queueFamilyProperties.size());

    for (uint32_t familyIndex = 0; familyIndex < queueFamilyProperties.size(); ++familyIndex)
    {
        const vk::QueueFamilyProperties &queueFamilyProperty = queueFamilyProperties[familyIndex];

        queuePriorities[familyIndex].resize(queueFamilyProperty.queueCount, 0.5f);

        vk::DeviceQueueCreateInfo &queueCreateInfo = queueCreateInfos[familyIndex];

        queueCreateInfo.queueFamilyIndex = familyIndex;
        queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
        queueCreateInfo.pQueuePriorities = queuePriorities[familyIndex].data();

        vk::Bool32 presentSupported = gpu.getHandle().getSurfaceSupportKHR(familyIndex, *surface);

        LOG_INFO("Found queue family {}: {}", familyIndex, getQueueFlagsString(queueFamilyProperty.queueFlags));
        for (uint32_t queueIndex = 0; queueIndex < queueFamilyProperty.queueCount; ++queueIndex)
        {
            queues[familyIndex].emplace_back(familyIndex, queueFamilyProperty, presentSupported, queueIndex);
        }
    }
    return queueCreateInfos;
}

void Device::createVmaAllocator(const Instance &instance, const PhysicalDevice &gpu)
{
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

VmaAllocator &Device::getVmaAllocator()
{
    return vmaAllocator;
}

const vk::raii::Device &Device::getHandle() const
{
    return handle;
}

const PhysicalDevice &Device::getPhysicalDevice() const
{
    return gpu;
}

Queue const &Device::getQueue(uint32_t queueIndex, uint32_t familyIndex) const
{
    return queues[familyIndex][queueIndex];
}

Queue const &Device::getQueueWithFlags(uint32_t queueIndex, vk::QueueFlags includeFlags,
                                       vk::QueueFlags excludeFlags) const
{
    for (const auto &family : queues)
    {
        for (const auto &queue : family)
        {
            vk::QueueFlags queueFlags = queue.getProperties().queueFlags;
            if ((queueFlags & includeFlags) == includeFlags && (queueFlags & excludeFlags) == vk::QueueFlags() &&
                queue.getQueueIndex() == queueIndex)
            {
                return queue;
            }
        }
    }
    if (excludeFlags == vk::QueueFlags())
    {
        LOG_ERROR("Queue not found with queue index: {}, include flags: {}", queueIndex,
                  getQueueFlagsString(includeFlags));
    }
    else
    {
        LOG_ERROR("Queue not found with queue index: {}, include flags: {}, exclude "
                  "flags: {}",
                  queueIndex, getQueueFlagsString(includeFlags), getQueueFlagsString(excludeFlags));
    }
    throw std::runtime_error("Queue not found");
}

Queue const &Device::getQueueWithPresent(uint32_t queueIndex, vk::QueueFlags excludeFlags) const
{
    for (const auto &family : queues)
    {
        for (const auto &queue : family)
        {
            vk::QueueFlags queueFlags = queue.getProperties().queueFlags;
            if (queue.canPresent() && (queueFlags & excludeFlags) == vk::QueueFlags() &&
                queue.getQueueIndex() == queueIndex)
            {
                return queue;
            }
        }
    }
    if (excludeFlags == vk::QueueFlags())
    {
        LOG_ERROR("Queue not found with queue index: {}", queueIndex);
    }
    else
    {
        LOG_ERROR("Queue not found with queue index: {}, exclude flags: {}", queueIndex,
                  getQueueFlagsString(excludeFlags));
    }
    throw std::runtime_error("Queue not found");
}

bool Device::hasQueue(uint32_t queueIndex, uint32_t familyIndex) const
{
    return familyIndex < queues.size() && queueIndex < queues[familyIndex].size();
}

bool Device::hasQueueWithFlags(uint32_t queueIndex, vk::QueueFlags includeFlags, vk::QueueFlags excludeFlags) const
{
    for (const auto &family : queues)
    {
        for (const auto &queue : family)
        {
            vk::QueueFlags queueFlags = queue.getProperties().queueFlags;
            if ((queueFlags & includeFlags) == includeFlags && (queueFlags & excludeFlags) == vk::QueueFlags() &&
                queue.getQueueIndex() == queueIndex)
            {
                return true;
            }
        }
    }
    return false;
}

bool Device::hasQueueWithPresent(uint32_t queueIndex, vk::QueueFlags excludeFlags) const
{
    for (const auto &family : queues)
    {
        for (const auto &queue : family)
        {
            vk::QueueFlags queueFlags = queue.getProperties().queueFlags;
            if (queue.canPresent() && (queueFlags & excludeFlags) == vk::QueueFlags() &&
                queue.getQueueIndex() == queueIndex)
            {
                return true;
            }
        }
    }
    return false;
}

} // namespace vkf::core