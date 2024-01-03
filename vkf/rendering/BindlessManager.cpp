////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file BindlessManager.cpp
/// \brief This file implements the BindlessManager class which is used for managing bindless resources in Vulkan.
///
/// The BindlessManager class is part of the vkf::rendering namespace. It provides functionality to store, update, and
/// remove buffers. It also provides methods to update descriptor sets and get pipeline layout and descriptor set.
///
/// \author Joshua Lowe
/// \date 12/15/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BindlessManager.h"
#include "../common/Log.h"
#include "../core/Device.h"

namespace vkf::rendering
{

BindlessManager::BindlessManager(const core::Device &device) : device{device}
{
    // Temporary DescriptorPool creation

    // Pool Sizes
    std::vector<vk::DescriptorPoolSize> poolSizes = {{vk::DescriptorType::eUniformBuffer, UniformCount},
                                                     {vk::DescriptorType::eStorageBuffer, StorageCount}};
    // Create descriptor pool
    auto createInfo = vk::DescriptorPoolCreateInfo{
        .flags =
            vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind | vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = 1,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };
    descriptorPool = vk::raii::DescriptorPool{device.getHandle(), createInfo};

    std::vector<vk::DescriptorSetLayoutBinding> bindings = {
        vk::DescriptorSetLayoutBinding{
            .binding = UniformBinding,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = UniformCount,
            .stageFlags = vk::ShaderStageFlagBits::eAll,
        },
        vk::DescriptorSetLayoutBinding{
            .binding = StorageBinding,
            .descriptorType = vk::DescriptorType::eStorageBuffer,
            .descriptorCount = StorageCount,
            .stageFlags = vk::ShaderStageFlagBits::eAll,
        },
    };

    std::vector<vk::DescriptorBindingFlags> bindingFlags = {
        vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateAfterBind,
        vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateAfterBind,
    };
    vk::DescriptorSetLayoutBindingFlagsCreateInfo setLayoutBindlingFlagsCreateInfo{
        .bindingCount = static_cast<uint32_t>(bindingFlags.size()),
        .pBindingFlags = bindingFlags.data(),
    };
    auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo{
        .pNext = &setLayoutBindlingFlagsCreateInfo,
        .flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    };
    descriptorSetLayouts.emplace_back(device.getHandle(), descriptorSetLayoutCreateInfo);

    auto descriptorSets = vk::raii::DescriptorSets{
        device.getHandle(),
        vk::DescriptorSetAllocateInfo{.descriptorPool = *descriptorPool,
                                      .descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
                                      .pSetLayouts = &*descriptorSetLayouts[0]}};
    descriptorSet = std::move(descriptorSets[0]);

    std::vector<vk::PushConstantRange> pushConstantRanges = {
        vk::PushConstantRange{
            .stageFlags = vk::ShaderStageFlagBits::eAll,
            .offset = 0,
            .size = 128,
        },
    };
    auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo{
        .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
        .pSetLayouts = &*descriptorSetLayouts[0],
        .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
        .pPushConstantRanges = pushConstantRanges.data(),
    };
    pipelineLayout = vk::raii::PipelineLayout{device.getHandle(), pipelineLayoutCreateInfo};
    LOG_INFO("BindlessManager created")
}

uint32_t BindlessManager::storeBuffer(core::Buffer &buffer, vk::BufferUsageFlags usage)
{
    uint32_t newHandle = buffers.size();

    if (!freeHandles.empty())
    {
        newHandle = freeHandles.back();
        freeHandles.pop_back();
    }
    else
    {
        newHandle = buffers.size();
    }
    buffers.emplace(newHandle, std::move(buffer));

    vk::DescriptorBufferInfo bufferInfo{
        .buffer = buffers.at(newHandle).getBuffer(),
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };
    descriptorBufferInfos.emplace_back(bufferInfo);

    vk::WriteDescriptorSet write{
        .dstSet = *descriptorSet,
        .dstArrayElement = newHandle,
        .descriptorCount = 1,
        .pBufferInfo = &descriptorBufferInfos.back(),
    };

    if ((usage & vk::BufferUsageFlagBits::eUniformBuffer) == vk::BufferUsageFlagBits::eUniformBuffer)
    {
        write.setDstBinding(UniformBinding);
        write.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    }

    if ((usage & vk::BufferUsageFlagBits::eStorageBuffer) == vk::BufferUsageFlagBits::eStorageBuffer)
    {
        write.setDstBinding(StorageBinding);
        write.setDescriptorType(vk::DescriptorType::eStorageBuffer);
    }

    writeDescriptorSets.emplace_back(write);
    device.getHandle().updateDescriptorSets(writeDescriptorSets, nullptr);
    writeDescriptorSets.clear();
    return newHandle;
}

void BindlessManager::updateBuffer(uint32_t handle, const void *data, uint32_t size, uint32_t offset)
{
    buffers.at(handle).updateData(data, size, offset);
}

void BindlessManager::removeBuffer(uint32_t handle)
{
    buffers.erase(handle);
    freeHandles.emplace_back(handle);
}

void BindlessManager::updateDescriptorSet()
{
    //    return;
    if (writeDescriptorSets.empty())
    {
        return;
    }
    device.getHandle().updateDescriptorSets(writeDescriptorSets, nullptr);
    writeDescriptorSets.clear();
}

vk::PipelineLayout BindlessManager::getPipelineLayout() const
{

    return *pipelineLayout;
}

vk::DescriptorSet BindlessManager::getDescriptorSet() const
{
    return *descriptorSet;
}

} // namespace vkf::rendering