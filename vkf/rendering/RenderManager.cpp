////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RenderManager.cpp
/// \brief This file implements the RenderManager class which is used for managing Vulkan rendering operations.
///
/// The RenderManager class is part of the vkf::rendering namespace. It provides an interface for interacting with
/// Vulkan rendering operations, including beginning and ending frames and render passes, drawing, and managing the
/// lifecycle of a Renderer object.
///
/// \author Joshua Lowe
/// \date 11/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RenderManager.h"
#include "../common/Log.h"
#include "../core/Device.h"
#include "../core/Framebuffer.h"
#include "../core/RenderPass.h"
#include "../core/Swapchain.h"
#include "../platform/Gui.h"
#include "../platform/Window.h"
#include "FrameData.h"
#include "RenderSubstage.h"
#include "Renderer.h"

namespace vkf::rendering
{

RenderManager::RenderManager(const core::Device &device, platform::Window &window,
                             std::shared_ptr<core::Swapchain> inputSwapchain,
                             std::vector<std::unique_ptr<Renderer>> inputRenderers)
    : device{device}, window{window}, renderers{std::move(inputRenderers)}, swapchain{std::move(inputSwapchain)}

{
    createFrameData();
    LOG_INFO("Created RenderManager")
}

RenderManager::~RenderManager() = default;

void RenderManager::beginFrame()
{

    device.getHandle().waitForFences(frameData[activeFrame]->getFences(), VK_TRUE,
                                     std::numeric_limits<uint64_t>::max());

    if (window.isResized())
    {
        recreateSwapchain();
    }

    auto [result, value] = swapchain->acquireNextImage(frameData[activeFrame]->getSemaphore(0));

    switch (result)
    {
    case vk::Result::eSuccess:
        //        LOG_DEBUG("Acquired next image: {}", value)
        break;
    case vk::Result::eSuboptimalKHR:
        LOG_DEBUG("Acquired next image: {} (suboptimal)", value)
        if (recreateSwapchain())
        {
            frameData[activeFrame]->refreshSemaphore(0);
            value = swapchain->acquireNextImage(frameData[activeFrame]->getSemaphore(0)).second;
        }
        break;
    case vk::Result::eErrorOutOfDateKHR:
        LOG_DEBUG("Acquired next image: {} (out of date)", value)
        if (recreateSwapchain())
        {
            frameData[activeFrame]->refreshSemaphore(0);
            value = swapchain->acquireNextImage(frameData[activeFrame]->getSemaphore(0)).second;
        }
        break;
    default:
        LOG_ERROR("Acquired next image: {} (error)", value)
        throw std::runtime_error{"Failed to acquire next image"};
    }

    imageIndex = value;
    activeCommandBuffers = frameData[activeFrame]->getCommandBuffers();

    frameActive = true;
    device.getHandle().resetFences(frameData[activeFrame]->getFences());
}

void RenderManager::endFrame()
{
    assert(frameActive && "Frame not active");

    auto presentQueue = device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics).getHandle();

    auto result =
        presentQueue.presentKHR(vk::PresentInfoKHR{.waitSemaphoreCount = 1,
                                                   .pWaitSemaphores = &*frameData[activeFrame]->getLastSemaphore(),
                                                   .swapchainCount = 1,
                                                   .pSwapchains = &*swapchain->getHandle(),
                                                   .pImageIndices = &imageIndex,
                                                   .pResults = nullptr});

    switch (result)
    {
    case vk::Result::eSuccess:
        //        LOG_DEBUG("Presented image: {}", value)
        break;
    case vk::Result::eSuboptimalKHR:
        //            LOG_DEBUG("Presented image: {} (suboptimal)", activeFrame)
        recreateSwapchain();
        break;
    case vk::Result::eErrorOutOfDateKHR:
        //            LOG_DEBUG("Presented image: {} (out of date)", activeFrame)
        recreateSwapchain();
        break;
    default:
        LOG_ERROR("Presented image: {} (error)", activeFrame)
        throw std::runtime_error{"Failed to present image"};
    }

    frameActive = false;
    activeFrame = ++activeFrame % framesInFlight;
}

void RenderManager::beginRenderPass(Renderer &renderer, uint32_t currentRenderPass)
{
    assert(frameActive && "Frame not active");
    activeCommandBuffers->at(currentRenderPass).reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    activeCommandBuffers->at(currentRenderPass)
        .begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    activeCommandBuffers->at(currentRenderPass)
        .beginRenderPass(renderer.getRenderPassBeginInfo(), vk::SubpassContents::eInline);
}

void RenderManager::endRenderPass(uint32_t currentRenderPass)
{
    assert(frameActive && "Frame not active");
    activeCommandBuffers->at(currentRenderPass).endRenderPass();
    activeCommandBuffers->at(currentRenderPass).end();

    auto presentQueue = device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics).getHandle();
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    presentQueue.submit(
        vk::SubmitInfo{.waitSemaphoreCount = 1,
                       .pWaitSemaphores = &*frameData[activeFrame]->getSemaphore(currentRenderPass),
                       .pWaitDstStageMask = waitStages,
                       .commandBufferCount = 1,
                       .pCommandBuffers = &(*activeCommandBuffers->at(currentRenderPass)), // What the hell
                       .signalSemaphoreCount = 1,
                       .pSignalSemaphores = &*frameData[activeFrame]->getSemaphore(currentRenderPass + 1)},
        frameData[activeFrame]->getFences().at(currentRenderPass));
}

void RenderManager::render()
{
    updateFrameBuffers();
    for (size_t i = 0; i < renderers.size(); ++i)
    {
        beginRenderPass(*renderers[i], i);
        renderers[i]->draw(&activeCommandBuffers->at(i)); // Pass the index to the draw function
        endRenderPass(i);
    }
}

void RenderManager::createFrameData()
{
    std::vector<FrameData *> renderFrameData;
    for (auto i = 0; i < framesInFlight; ++i)
    {
        frameData.emplace_back(std::make_unique<FrameData>(device, renderers.size()));
        renderFrameData.emplace_back(frameData.back().get());
    }
    LOG_INFO("Created FrameData x{}", framesInFlight)
}

void RenderManager::updateFrameBuffers()
{
    for (auto &renderer : renderers)
    {
        renderer->updateFramebuffers();
    }
}

void RenderManager::syncFrameData()
{
    for (const auto &frame : frameData)
    {
        device.getHandle().waitForFences(frame->getFences(), VK_TRUE, std::numeric_limits<uint64_t>::max());
    }
}

bool RenderManager::recreateSwapchain()
{
    syncFrameData();

    swapchain->recreate();

    window.setResized(false);

    return true;
}

} // namespace vkf::rendering