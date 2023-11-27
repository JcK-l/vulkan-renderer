////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Renderer.cpp
/// \brief This file implements the Renderer class which is used for managing Vulkan rendering operations.
///
/// The Renderer class is part of the vkf::rendering namespace. It provides an interface for interacting with Vulkan
/// rendering operations.
///
/// \author Joshua Lowe
/// \date 11/20/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Renderer.h"
#include "../common/Log.h"
#include "../core/RenderPass.h"
#include "../core/Swapchain.h"
#include "RenderSubstage.h"

namespace vkf::rendering
{

Renderer::Renderer(const core::Device &device, core::Swapchain *swapchain, RenderOptions renderOptions)
    : device{device}, swapchain{swapchain}, renderOptions{std::make_unique<RenderOptions>(renderOptions)}
{
    createRenderTargets();
    createRenderSubstages();

    std::vector<vk::SubpassDescription> subpassDescriptions(renderOptions.numSubpasses);

    std::vector<vk::AttachmentReference> attachments(renderOptions.numSubpasses);
    attachments[0] = vk::AttachmentReference{0, vk::ImageLayout::eColorAttachmentOptimal};

    for (auto i = 0u; i < renderOptions.numSubpasses; ++i)
    {
        subpassDescriptions[i].colorAttachmentCount = 1;
        subpassDescriptions[i].pColorAttachments = attachments.data();
        subpassDescriptions[i].pDepthStencilAttachment = nullptr;
        subpassDescriptions[i].pInputAttachments = nullptr;
        subpassDescriptions[i].pPreserveAttachments = nullptr;
        subpassDescriptions[i].pResolveAttachments = nullptr;
        subpassDescriptions[i].pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpassDescriptions[i].flags = {};
    }

    std::vector<vk::SubpassDependency> dependencies(renderOptions.numSubpasses - 1);
    for (auto i = 0u; i < renderOptions.numSubpasses - 1; ++i)
    {
        dependencies[i].srcSubpass = i;
        dependencies[i].dstSubpass = i + 1;
        dependencies[i].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependencies[i].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dependencies[i].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        dependencies[i].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        dependencies[i].dependencyFlags = {};
    }

    renderPass = std::make_unique<core::RenderPass>(device, *renderTargets[0], subpassDescriptions, dependencies);

    LOG_INFO("Created Renderer")
}

Renderer::~Renderer() = default;

void Renderer::draw(vk::CommandBuffer *cmd)
{
    for (auto i = 0u; i < renderOptions->numSubpasses; ++i)
    {
        renderSubstages[i]->draw(cmd);
        if (i < renderOptions->numSubpasses - 1)
            cmd->nextSubpass(vk::SubpassContents::eInline);
    }
}

void Renderer::updateRenderTargets()
{
    for (auto i = 0u; i < swapchain->getImageCount(); ++i)
    {
        renderTargets[i]->extent = swapchain->getExtent();
        renderTargets[i]->imageViews.back() = swapchain->createImageView(i);
    }
}

core::RenderPass *Renderer::getRenderPass() const
{
    return renderPass.get();
}

vk::ClearValue *Renderer::getClearValue() const
{
    return &renderOptions->clearValue;
}

const RenderTarget &Renderer::getRenderTarget(uint32_t index)
{
    return *renderTargets.at(index);
}

void Renderer::createRenderTargets()
{
    renderTargets.clear();
    renderTargets.reserve(swapchain->getImageCount());
    for (auto i = 0u; i < swapchain->getImageCount(); ++i)
    {
        std::vector<vk::raii::ImageView> imageViews;
        imageViews.emplace_back(swapchain->createImageView(i));
        // more image views later

        std::vector<vk::AttachmentDescription> attachments;
        attachments.emplace_back(vk::AttachmentDescription{
            .format = vk::Format::eR8G8B8A8Srgb,                // Assuming the swapchain image format is R8G8B8A8 srgb
            .samples = vk::SampleCountFlagBits::e1,             // Single sample, as multi-sampling is not used
            .loadOp = vk::AttachmentLoadOp::eClear,             // Clear the image at the start
            .storeOp = vk::AttachmentStoreOp::eStore,           // Store the image to memory after rendering
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,   // We don't care about stencil
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare, // We don't care about stencil
            .initialLayout = vk::ImageLayout::eUndefined,       // We don't care about the initial layout
            .finalLayout = vk::ImageLayout::ePresentSrcKHR      // Image will be used as source for presentation
        });

        renderTargets.emplace_back(
            std::make_unique<RenderTarget>(swapchain->getExtent(), std::move(imageViews), attachments));
    }
}

void Renderer::createRenderSubstages()
{
    for (auto i = 0u; i < renderOptions->numSubpasses; ++i)
    {
        renderSubstages.emplace_back();
    }
}

} // namespace vkf::rendering