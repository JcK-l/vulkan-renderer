////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file BindlessManager.h
/// \brief This file declares the BindlessManager class which is used for managing bindless resources in Vulkan.
///
/// The BindlessManager class is part of the vkf::rendering namespace. It provides functionality to store, update, and
/// remove buffers. It also provides methods to update descriptor sets and get pipeline layout and descriptor set.
///
/// \author Joshua Lowe
/// \date 12/15/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_BINDLESSMANAGER_H
#define VULKANRENDERER_BINDLESSMANAGER_H

#include "../core/Buffer.h"

namespace vkf::core // Forward declaration
{
class Device;
} // namespace vkf::core

namespace vkf::rendering
{

///
/// \class BindlessManager
/// \brief Class for managing bindless resources in Vulkan.
///
/// This class provides functionality to store, update, and remove buffers. It also provides methods to update
/// descriptor sets and get pipeline layout and descriptor set.
///
class BindlessManager
{
  public:
    ///
    /// \brief Constructor that takes a device as parameter.
    ///
    /// \param device The device to use for creating the BindlessManager.
    ///
    explicit BindlessManager(const core::Device &device);

    BindlessManager(const BindlessManager &) = delete;            ///< Deleted copy constructor
    BindlessManager(BindlessManager &&) noexcept = default;       ///< Default move constructor
    BindlessManager &operator=(const BindlessManager &) = delete; ///< Deleted copy assignment operator
    BindlessManager &operator=(BindlessManager &&) = delete;      ///< Deleted move assignment operator
    ~BindlessManager() = default;                                 ///< Default destructor

    ///
    /// \brief Method to store a buffer.
    ///
    /// This method takes a buffer and its usage, and returns a handle to the stored buffer.
    ///
    uint32_t storeBuffer(core::Buffer &buffer, vk::BufferUsageFlags usage);

    ///
    /// \brief Method to update a buffer.
    ///
    /// This method takes a handle to a buffer, data to update, size of the data, and offset in the buffer, and updates
    /// the buffer.
    ///
    void updateBuffer(uint32_t handle, const void *data, uint32_t size, uint32_t offset);

    ///
    /// \brief Method to remove a buffer.
    ///
    /// This method takes a handle to a buffer and removes the buffer.
    ///
    void removeBuffer(uint32_t handle);

    ///
    /// \brief Method to update descriptor set.
    ///
    /// This method updates the descriptor set.
    ///
    void updateDescriptorSet();

    [[nodiscard]] vk::PipelineLayout getPipelineLayout() const;
    [[nodiscard]] vk::DescriptorSet getDescriptorSet() const;

    static constexpr uint32_t UniformBinding = 0;   ///< Uniform binding index
    static constexpr uint32_t StorageBinding = 1;   ///< Storage binding index
    static constexpr uint32_t UniformCount = 65536; ///< Maximum number of uniform buffers
    static constexpr uint32_t StorageCount = 65536; ///< Maximum number of storage buffers

  private:
    const core::Device &device;

    vk::raii::DescriptorPool descriptorPool = VK_NULL_HANDLE;
    vk::raii::DescriptorSet descriptorSet = VK_NULL_HANDLE;
    std::vector<vk::raii::DescriptorSetLayout> descriptorSetLayouts;
    vk::raii::PipelineLayout pipelineLayout = VK_NULL_HANDLE;

    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos;
    std::unordered_map<uint32_t, core::Buffer> buffers; ///< Map of handles and their corresponding buffers
    std::vector<uint32_t> freeHandles;                  ///< Vector of free handles
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_BINDLESSMANAGER_H