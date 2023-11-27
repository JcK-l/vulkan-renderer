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
#include "../platform/Window.h"
#include "FrameData.h"
#include "RenderSubstage.h"
#include "Renderer.h"

namespace vkf::rendering
{

RenderManager::RenderManager(const core::Device &device, platform::Window &window,
                             std::unique_ptr<core::Swapchain> inputSwapchain, std::unique_ptr<Renderer> inputRenderer)
    : device{device}, window{window}, renderer{std::move(inputRenderer)}, renderPass{renderer->getRenderPass()},
      swapchain{std::move(inputSwapchain)}
{
    createFrameData();
    createFramebuffers();
    LOG_INFO("Created RenderManager")
}

RenderManager::~RenderManager() = default;

void RenderManager::beginFrame()
{
    device.getHandle().waitForFences(*frameData[activeFrame]->getFence(), VK_TRUE,
                                     std::numeric_limits<uint64_t>::max());

    auto [result, value] = swapchain->acquireNextImage(frameData[activeFrame]->getSemaphore(0));
    switch (result)
    {
    case vk::Result::eSuccess:
        //        LOG_DEBUG("Acquired next image: {}", value)
        break;
    case vk::Result::eSuboptimalKHR:
        LOG_DEBUG("Acquired next image: {} (suboptimal)", value)
        if (recreateSwapchain())
            value =
                swapchain->acquireNextImage(vk::raii::Semaphore{device.getHandle(), vk::SemaphoreCreateInfo{}}).second;
        break;
    default:
        LOG_ERROR("Acquired next image: {} (error)", value)
        throw std::runtime_error{"Failed to acquire next image"};
    }

    imageIndex = value;
    activeCommandBuffer = frameData[activeFrame]->getCommandBuffer();

    frameActive = true;
    device.getHandle().resetFences(*frameData[activeFrame]->getFence());
}

void RenderManager::endFrame()
{
    assert(frameActive && "Frame not active");

    auto presentQueue = device.getQueueWithFlags(0, vk::QueueFlagBits::eGraphics).getHandle();
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    presentQueue.submit(vk::SubmitInfo{.waitSemaphoreCount = 1,
                                       .pWaitSemaphores = &*frameData[activeFrame]->getSemaphore(0),
                                       .pWaitDstStageMask = waitStages,
                                       .commandBufferCount = 1,
                                       .pCommandBuffers = &*(*activeCommandBuffer), // What the hell
                                       .signalSemaphoreCount = 1,
                                       .pSignalSemaphores = &*frameData[activeFrame]->getSemaphore(1)},
                        *frameData[activeFrame]->getFence());

    auto result =
        presentQueue.presentKHR(vk::PresentInfoKHR{.waitSemaphoreCount = 1,
                                                   .pWaitSemaphores = &*frameData[activeFrame]->getSemaphore(1),
                                                   .swapchainCount = 1,
                                                   .pSwapchains = &*swapchain->getHandle(),
                                                   .pImageIndices = &imageIndex,
                                                   .pResults = nullptr});

    if (window.isResized())
    {
        recreateSwapchain();
    }
    else
    {
        switch (result)
        {
        case vk::Result::eSuccess:
            //        LOG_DEBUG("Presented image: {}", value)
            break;
        case vk::Result::eSuboptimalKHR:
            LOG_DEBUG("Presented image: {} (suboptimal)", activeFrame)
            recreateSwapchain();
            break;
        case vk::Result::eErrorOutOfDateKHR:
            LOG_DEBUG("Presented image: {} (out of date)", activeFrame)
            recreateSwapchain();
            break;
        default:
            LOG_ERROR("Presented image: {} (error)", activeFrame)
            throw std::runtime_error{"Failed to present image"};
        }
    }

    frameActive = false;
    activeFrame = activeFrame++ % framesInFlight;
}

void RenderManager::beginRenderPass()
{
    assert(frameActive && "Frame not active");
    activeCommandBuffer->reset(vk::CommandBufferResetFlagBits::eReleaseResources);
    activeCommandBuffer->begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    vk::RenderPassBeginInfo renderPassBeginInfo{.renderPass = *renderPass->getHandle(),
                                                .framebuffer = *framebuffers[imageIndex]->getHandle(),
                                                .renderArea = vk::Rect2D{{0, 0}, swapchain->getExtent()},
                                                .clearValueCount = 1,
                                                .pClearValues = renderer->getClearValue()};

    activeCommandBuffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void RenderManager::endRenderPass()
{
    assert(frameActive && "Frame not active");
    activeCommandBuffer->endRenderPass();
    activeCommandBuffer->end();
}

void RenderManager::draw()
{
    assert(frameActive && "Frame not active");
}

void RenderManager::createFramebuffers()
{
    framebuffers.clear();
    framebuffers.reserve(swapchain->getImageCount());
    for (auto i = 0; i < swapchain->getImageCount(); ++i)
    {
        framebuffers.emplace_back(
            std::make_unique<core::Framebuffer>(device, *renderPass, renderer->getRenderTarget(i)));
    }
    LOG_INFO("Created Framebuffer x{}", framesInFlight)
}

void RenderManager::createFrameData()
{
    for (auto i = 0; i < framesInFlight; ++i)
    {
        frameData.emplace_back(std::make_unique<FrameData>(device));
    }
    LOG_INFO("Created FrameData x{}", framesInFlight)
}

bool RenderManager::recreateSwapchain()
{
    swapchain->recreate();

    renderer->updateRenderTargets();

    createFramebuffers();
    window.setResized(false);

    return true;
}

} // namespace vkf::rendering