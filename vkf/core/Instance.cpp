////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Instance.cpp
/// \brief This file implements the Instance class which is used to manage Vulkan instances.
///
/// The Instance class is part of the vkf::core namespace. It provides methods for creating a Vulkan instance,
/// validating extensions and layers, and querying GPUs. In debug build mode, it also creates a debug messenger.
///
/// \author Joshua Lowe
/// \date 11/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Instance.h"
#include "../common/Log.h"
#include "PhysicalDevice.h"

#if !defined(NDEBUG)
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                             VkDebugUtilsMessengerCallbackDataEXT const *callbackData,
                                             void * /*pUserData*/)
{
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        LOG_WARN("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage)
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LOG_ERROR("{} - {}: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage)
    }
    return VK_FALSE;
}

#endif

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace vkf::core
{

Instance::Instance(const std::string &appName, const std::vector<const char *> &requiredExtensions,
                   const std::vector<const char *> &requiredLayers)
{
    vk::DynamicLoader dl;
    auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    availableExtensions = context.enumerateInstanceExtensionProperties();
    validateExtensions(requiredExtensions);

    availableLayers = context.enumerateInstanceLayerProperties();
    validateLayers(requiredLayers);

    vk::ApplicationInfo applicationInfo{.pApplicationName = appName.c_str(),
                                        .applicationVersion = 1,
                                        .pEngineName = "engine",
                                        .engineVersion = 1,
                                        .apiVersion = VK_API_VERSION_1_3};

#if !defined(NDEBUG)
    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> instanceCreateInfo = {
        vk::InstanceCreateInfo{.pApplicationInfo = &applicationInfo,
                               .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
                               .ppEnabledLayerNames = enabledLayers.data(),
                               .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
                               .ppEnabledExtensionNames = enabledExtensions.data()},
        vk::DebugUtilsMessengerCreateInfoEXT{
            .messageSeverity =
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                           vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            .pfnUserCallback = &debugCallback,
        }};
#else
    vk::StructureChain<vk::InstanceCreateInfo> instanceCreateInfo = {
        vk::InstanceCreateInfo{.pApplicationInfo = &applicationInfo,
                               .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
                               .ppEnabledLayerNames = enabledLayers.data(),
                               .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
                               .ppEnabledExtensionNames = enabledExtensions.data()}};
#endif
    handle = vk::raii::Instance{context, instanceCreateInfo.get<vk::InstanceCreateInfo>()};
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*handle);
    LOG_INFO("Created Instance")

#if !defined(NDEBUG)
    debugMessenger = vk::raii::DebugUtilsMessengerEXT{
        handle, vk::DebugUtilsMessengerCreateInfoEXT{
                    .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                       vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                    .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                    .pfnUserCallback = &debugCallback,
                }};
    LOG_DEBUG("Created DebugMessenger")
#endif
    queryGpus();
}

Instance::~Instance() = default;

bool Instance::enableExtension(const char *requiredExtensionName)
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
            LOG_WARN("Instance extension {} is already enabled", requiredExtensionName)
        }
        else
        {
            LOG_INFO("Enabling instance extension: {}", requiredExtensionName)
            enabledExtensions.emplace_back(requiredExtensionName);
        }
        return true;
    }

    LOG_WARN("Instance extension {} not found", requiredExtensionName)
    return false;
}

bool Instance::enableLayer(const char *requiredLayerName)
{
    auto availableLayerIt =
        std::find_if(availableLayers.begin(), availableLayers.end(), [requiredLayerName](const auto &availableLayer) {
            return strcmp(availableLayer.layerName, requiredLayerName) == 0;
        });

    if (availableLayerIt != availableLayers.end())
    {
        auto enabledLayerIt =
            std::find_if(enabledLayers.begin(), enabledLayers.end(), [requiredLayerName](const char *enabledLayerName) {
                return strcmp(enabledLayerName, requiredLayerName) == 0;
            });

        if (enabledLayerIt != enabledLayers.end())
        {
            LOG_WARN("Instance layer {} is already enabled", requiredLayerName)
        }
        else
        {
            LOG_INFO("Enabling instance layer: {}", requiredLayerName)
            enabledLayers.emplace_back(requiredLayerName);
        }
        return true;
    }

    LOG_WARN("Instance layer {} not found", requiredLayerName)
    return false;
}

void Instance::validateExtensions(const std::vector<const char *> &requiredExtensions)
{
    for (auto extension : requiredExtensions)
    {
        if (!enableExtension(extension))
        {
            LOG_ERROR("Required instance extension {} is not available", extension)
            throw std::runtime_error("Required instance extensions are missing.");
        }
    }
}

void Instance::validateLayers(const std::vector<const char *> &requiredLayers)
{
    for (auto layer : requiredLayers)
    {
        if (!enableLayer(layer))
        {
            LOG_ERROR("Required instance layer {} is not available", layer)
            throw std::runtime_error("Required instance layers are missing.");
        }
    }
}

void Instance::queryGpus()
{
    vk::raii::PhysicalDevices physicalDevices{handle};
    if (physicalDevices.empty())
    {
        throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
    }

    // Create GPUs wrapper objects from the vk::raii::PhysicalDevice's
    for (auto &physicalDevice : physicalDevices)
    {
        gpus.push_back(std::make_unique<PhysicalDevice>(physicalDevice));
    }
}

PhysicalDevice &Instance::getSuitableGpu(vk::raii::SurfaceKHR &surface)
{
    assert(!gpus.empty() && "No physical devices found");

    for (const auto &gpu : gpus)
    {
        auto queueFamilyProperties = gpu->getQueueFamilyProperties();
        auto size = queueFamilyProperties.size();

        if (gpu->getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {

            for (auto i = 0; i < size; ++i)
            {
                if (gpu->getSurfaceSupportKHR(i, *surface))
                {
                    return *gpu;
                }
            }
        }
    }
    LOG_WARN("No suitable gpu found. Picking first GPU: {}", gpus[0]->getProperties().deviceName.data())
    return *gpus[0];
}

const vk::raii::Instance &Instance::getHandle() const
{
    return handle;
}

const vk::raii::Context &Instance::getContext() const
{
    return context;
}
} // namespace vkf::core