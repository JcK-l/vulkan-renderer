////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file FrameData.h
/// \brief This file declares the FrameData class which is used for managing frame data in Vulkan.
///
/// The FrameData class is part of the vkf::rendering namespace. It provides an interface for interacting with frame
/// data, including getting the command buffer.
///
/// \author Joshua Lowe
/// \date 11/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_FRAMEDATA_H
#define VULKANRENDERER_FRAMEDATA_H

namespace vkf::core // Forward declarations
{
class Device;
class Swapchain;
class CommandPool;
} // namespace vkf::core

namespace vkf::rendering
{

///
/// \class FrameData
/// \brief This class manages per frame data.
///
/// It provides an interface for interacting with frame data, including getting the command buffer.
///
class FrameData
{
  public:
    ///
    /// \brief Constructs a FrameData object.
    ///
    /// This constructor creates frame data using the provided device
    ///
    /// \param device The Vulkan device to use for creating the frame data.
    ///
    FrameData(const core::Device &device, uint32_t numRenderPasses);

    FrameData(const FrameData &) = delete;            ///< Deleted copy constructor
    FrameData(FrameData &&) noexcept = default;       ///< Default move constructor
    FrameData &operator=(const FrameData &) = delete; ///< Deleted copy assignment operator
    FrameData &operator=(FrameData &&) = delete;      ///< Deleted move assignment operator
    ~FrameData(); ///< Implementation in FrameData.cpp because of std::unique_ptr forward declaration

    vk::raii::CommandBuffers *getCommandBuffers();

    void refreshSemaphore(uint32_t index);
    [[nodiscard]] const vk::raii::Semaphore &getSemaphore(uint32_t index) const;
    [[nodiscard]] const vk::raii::Semaphore &getLastSemaphore() const;
    [[nodiscard]] std::vector<vk::Fence> getFences() const;

  private:
    const core::Device &device;

    std::unique_ptr<core::CommandPool> commandPool;
    std::unordered_map<uint32_t, vk::raii::CommandBuffers *> commandBuffersMap;

    std::vector<vk::raii::Semaphore> semaphore;
    std::vector<vk::raii::Fence> fences;
};

} // namespace vkf::rendering

#endif // VULKANRENDERER_FRAMEDATA_H