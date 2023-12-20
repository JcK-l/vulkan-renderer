/// \file
/// \brief

//
// Created by Joshua Lowe on 12/15/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_BINDLESSMANAGER_H
#define VULKANRENDERER_BINDLESSMANAGER_H

#include "../core/Buffer.h"

namespace vkf::core // Forward declaration
{
class Device;
} // namespace vkf::core

namespace vkf::rendering
{

class BindlessManager
{
  public:
    BindlessManager(const core::Device &device);
    BindlessManager(const BindlessManager &) = delete;            // Deleted copy constructor
    BindlessManager(BindlessManager &&) noexcept = default;       // Default move constructor
    BindlessManager &operator=(const BindlessManager &) = delete; // Deleted copy assignment operator
    BindlessManager &operator=(BindlessManager &&) = delete;      // Deleted move assignment operator
    ~BindlessManager() = default;                                 // Default destructor

    uint32_t storeBuffer(core::Buffer &buffer, vk::BufferUsageFlags usage);
    void updateBuffer(uint32_t handle, const void *data, uint32_t size, uint32_t offset);
    void removeBuffer(uint32_t handle);

    void updateDescriptorSet();

    [[nodiscard]] vk::PipelineLayout getPipelineLayout() const;
    [[nodiscard]] vk::DescriptorSet getDescriptorSet() const;

    static constexpr uint32_t UniformBinding = 0;
    static constexpr uint32_t StorageBinding = 1;
    static constexpr uint32_t UniformCount = 65536;
    static constexpr uint32_t StorageCount = 65536;

  private:
    const core::Device &device;

    vk::raii::DescriptorPool descriptorPool = VK_NULL_HANDLE;
    vk::raii::DescriptorSet descriptorSet = VK_NULL_HANDLE;
    std::vector<vk::raii::DescriptorSetLayout> descriptorSetLayouts;
    vk::raii::PipelineLayout pipelineLayout = VK_NULL_HANDLE;

    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
    std::unordered_map<uint32_t, core::Buffer> buffers;
    std::vector<uint32_t> freeHandles;
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_BINDLESSMANAGER_H