////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file CommandPool.h
/// \brief This file declares the CommandPool class which is used for managing Vulkan command pools.
///
/// The CommandPool class is part of the vkf::core namespace. It provides functionality to get the handle to the command
/// pool and request command buffers.
///
/// \author Joshua Lowe
/// \date 11/20/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_COMMANDPOOL_H
#define VULKANRENDERER_COMMANDPOOL_H

namespace vkf::core
{

// Forward declarations
class Device;

///
/// \class CommandPool
/// \brief This class manages Vulkan command pools.
///
/// It provides functionality to get the handle to the command pool and request command buffers.
///
class CommandPool
{
  public:
    ///
    /// \brief Constructor for the CommandPool class.
    ///
    /// \param device The Vulkan device.
    /// \param createInfo The information for creating a command pool.
    ///
    explicit CommandPool(const Device &device, vk::CommandPoolCreateInfo createInfo);

    CommandPool(const CommandPool &) = delete;            ///< Deleted copy constructor
    CommandPool(CommandPool &&) noexcept = default;       ///< Default move constructor
    CommandPool &operator=(const CommandPool &) = delete; ///< Deleted copy assignment operator
    CommandPool &operator=(CommandPool &&) = delete;      ///< Deleted move assignment operator
    ~CommandPool() = default;                             ///< Default destructor

    [[nodiscard]] const vk::raii::CommandPool &getHandle() const;

    ///
    /// \brief Request command buffers.
    ///
    /// This function requests command buffers from the command pool and stores them in an unordered map
    /// for later reference and automatic cleanup.
    /// see https://github.com/KhronosGroup/Vulkan-Hpp/blob/main/vk_raii_ProgrammingGuide.md section 4 for more info.
    ///
    /// \param level The level of the command buffers.
    /// \param count The number of command buffers.
    /// \return A pair containing the key and a raw pointer to command buffers. Ownership is not transferred!
    ///
    std::pair<uint32_t, vk::raii::CommandBuffers *> requestCommandBuffers(vk::CommandBufferLevel level, uint32_t count);

    std::optional<vk::raii::CommandBuffers *> getCommandBuffers(uint32_t key) const;

  private:
    const Device &device;

    vk::raii::CommandPool handle{VK_NULL_HANDLE};
    uint32_t allocations{0};
    std::unordered_map<uint32_t, std::unique_ptr<vk::raii::CommandBuffers>> commandBuffersMap;
};

} // namespace vkf::core

#endif // VULKANRENDERER_COMMANDPOOL_H