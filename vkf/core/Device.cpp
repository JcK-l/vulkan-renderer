////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Device.cpp
/// \brief This file implements the Device class which is used to manage Vulkan devices.
///
/// The Device class is part of the vkf::core namespace. It provides functionality to get queues and handle to the
/// device. It also provides methods to check if a queue exists and to get the VMA allocator.
///
/// \author Joshua Lowe
/// \date 11/9/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Device.h"
#include "../common/Log.h"
#include "CommandPool.h"
#include "Instance.h"
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
    LOG_INFO("Picked GPU: {}", gpu.getProperties().deviceName.data())

    availableExtensions = gpu.getHandle().enumerateDeviceExtensionProperties();
    validateExtensions(requiredExtensions);
    enableExtension("VK_KHR_portability_subset"); // only necessary for macOS and MoltenVK

    auto feature = gpu.requestExtensionFeatures<vk::PhysicalDeviceDescriptorIndexingFeatures>();
    assert(feature.shaderSampledImageArrayNonUniformIndexing &&
           "Device does not support shaderSampledImageArrayNonUniformIndexing");
    assert(feature.descriptorBindingSampledImageUpdateAfterBind &&
           "Device does not support descriptorBindingSampledImageUpdateAfterBind");
    assert(feature.shaderUniformBufferArrayNonUniformIndexing &&
           "Device does not support shaderUniformBufferArrayNonUniformIndexing");
    assert(feature.descriptorBindingUniformBufferUpdateAfterBind &&
           "Device does not support descriptorBindingUniformBufferUpdateAfterBind");
    assert(feature.shaderStorageBufferArrayNonUniformIndexing &&
           "Device does not support shaderStorageBufferArrayNonUniformIndexing");
    assert(feature.descriptorBindingStorageBufferUpdateAfterBind &&
           "Device does not support descriptorBindingStorageBufferUpdateAfterBind");

    createQueuesInfos();
    vk::DeviceCreateInfo createInfo{.pNext = gpu.getExtensionFeaturesHead(),
                                    .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                                    .pQueueCreateInfos = queueCreateInfos.data(),
                                    .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
                                    .ppEnabledExtensionNames = enabledExtensions.data(),
                                    .pEnabledFeatures = &gpu.getPhysicalDeviceFeatures()};

    handle = vk::raii::Device{gpu.getHandle(), createInfo};
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*handle);
    LOG_INFO("Created Device")

    createQueues();

    createVmaAllocator(instance, gpu);

    // This commandPool could use an exclusive transfer queue instead. But that would require queue ownership transfer
    commandPool = std::make_unique<CommandPool>(
        *this,
        vk::CommandPoolCreateInfo{
            .flags = vk::CommandPoolCreateFlags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
            .queueFamilyIndex = getQueueWithFlags(0, vk::QueueFlagBits::eGraphics, vk::QueueFlags()).getFamilyIndex()});

    commandBuffers = commandPool->requestCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).second;
}

Device::~Device()
{
    if (vmaAllocator)
    {
        vmaDestroyAllocator(vmaAllocator);
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

    // VK_KHR_portability_subset not found means that the implementation is fully Vulkan conformant
    if (strcmp(requiredExtensionName, "VK_KHR_portability_subset") == 0)
    {
        LOG_INFO("Vulkan implementation on the GPU is fully conformant")
        return false;
    }

    LOG_WARN("Device extension {} not found", requiredExtensionName)
    return false;
}

void Device::createQueuesInfos()
{
    auto queueFamilyProperties = gpu.getQueueFamilyProperties();
    queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>(queueFamilyProperties.size());
    queuePriorities = std::vector<std::vector<float>>(queueFamilyProperties.size());

    queues.resize(queueFamilyProperties.size());

    uint32_t familyIndex = 0;
    std::for_each(queueFamilyProperties.begin(), queueFamilyProperties.end(),
                  [&](const vk::QueueFamilyProperties &queueFamilyProperty) {
                      queuePriorities[familyIndex].resize(queueFamilyProperty.queueCount, 0.5f);

                      vk::DeviceQueueCreateInfo &queueCreateInfo = queueCreateInfos[familyIndex];
                      queueCreateInfo.queueFamilyIndex = familyIndex;
                      queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
                      queueCreateInfo.pQueuePriorities = queuePriorities[familyIndex].data();

                      ++familyIndex;
                  });
}

void Device::createQueues()
{
    auto queueFamilyProperties = gpu.getQueueFamilyProperties();
    queues.resize(queueFamilyProperties.size());

    uint32_t familyIndex = 0;
    std::for_each(queueFamilyProperties.begin(), queueFamilyProperties.end(),
                  [&](const vk::QueueFamilyProperties &queueFamilyProperty) {
                      auto presentSupported = gpu.getHandle().getSurfaceSupportKHR(familyIndex, *surface);

                      LOG_INFO("Found queue family {}: {}", familyIndex,
                               getQueueFlagsString(queueFamilyProperty.queueFlags))
                      for (uint32_t queueIndex = 0; queueIndex < queueFamilyProperty.queueCount; ++queueIndex)
                      {
                          queues[familyIndex].emplace_back(handle.getQueue(familyIndex, queueIndex), familyIndex,
                                                           queueFamilyProperty, presentSupported, queueIndex);
                      }
                      ++familyIndex;
                  });
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
    VkResult result = vmaCreateAllocator(&vmaCreteInfo, &vmaAllocator);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create VMA allocator");
    }
    LOG_INFO("Created VMA allocator")
}

const VmaAllocator &Device::getVmaAllocator() const
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

vk::raii::CommandBuffers *Device::getCommandBuffers() const
{
    return commandBuffers;
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
                  getQueueFlagsString(includeFlags))
    }
    else
    {
        LOG_ERROR("Queue not found with queue index: {}, include flags: {}, exclude "
                  "flags: {}",
                  queueIndex, getQueueFlagsString(includeFlags), getQueueFlagsString(excludeFlags))
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
        LOG_ERROR("Queue not found with queue index: {}", queueIndex)
    }
    else
    {
        LOG_ERROR("Queue not found with queue index: {}, exclude flags: {}", queueIndex,
                  getQueueFlagsString(excludeFlags))
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