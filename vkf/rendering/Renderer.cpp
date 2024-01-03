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
#include "../core/Device.h"
#include "../core/Framebuffer.h"
#include "../core/RenderPass.h"
#include "FrameData.h"
#include "RenderSubstage.h"
#include <utility>

namespace vkf::rendering
{

Renderer::Renderer(const core::Device &device, RenderOptions inputRenderOptions,
                   std::shared_ptr<RenderSource> renderSource)
    : device{device}, renderOptions{std::move(inputRenderOptions)}, renderSource{std::move(renderSource)}
{
    std::vector<vk::SubpassDescription> subpassDescriptions(renderOptions.numSubpasses);

    std::vector<vk::AttachmentReference> attachmentRefs(renderOptions.numSubpasses);
    attachmentRefs[0] = vk::AttachmentReference{0, vk::ImageLayout::eColorAttachmentOptimal};

    vk::AttachmentReference depthAttachmentRef =
        (renderOptions.useDepth)
            ? vk::AttachmentReference{1, vk::ImageLayout::eDepthStencilAttachmentOptimal}
            : vk::AttachmentReference{VK_ATTACHMENT_UNUSED, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    for (auto i = 0u; i < renderOptions.numSubpasses; ++i)
    {
        subpassDescriptions[i].colorAttachmentCount = 1;
        subpassDescriptions[i].pColorAttachments = &attachmentRefs[i];
        subpassDescriptions[i].pDepthStencilAttachment = &depthAttachmentRef;
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

    renderPass =
        std::make_unique<core::RenderPass>(device, renderOptions.attachments, subpassDescriptions, dependencies);

    LOG_INFO("Created Renderer")
}

Renderer::~Renderer() = default;

void Renderer::draw(vk::raii::CommandBuffer *cmd)
{
    for (auto i = 0u; i < renderOptions.numSubpasses; ++i)
    {
        renderSubstages[i]->draw(cmd);
        if (i < renderOptions.numSubpasses - 1)
            cmd->nextSubpass(vk::SubpassContents::eInline);
    }
}

void Renderer::addRenderSubstage(std::unique_ptr<RenderSubstage> renderSubstage)
{
    auto substageType = renderSubstage->getType();
    LOG_INFO("Added {} RenderSubstage", substageType)
    renderSubstages.emplace_back(std::move(renderSubstage));
}

void Renderer::updateFramebuffers()
{
    if (renderSource->resetChanged())
    {
        framebufferExtent = renderSource->getExtent();
        createFramebuffers(renderSource->getImageViews());
    }
}

core::RenderPass *Renderer::getRenderPass() const
{
    return renderPass.get();
}

vk::RenderPassBeginInfo Renderer::getRenderPassBeginInfo(uint32_t imageIndex) const
{
    return vk::RenderPassBeginInfo{.renderPass = *renderPass->getHandle(),
                                   .framebuffer = *framebuffers[imageIndex]->getHandle(),
                                   .renderArea = vk::Rect2D{{0, 0}, framebufferExtent},
                                   .clearValueCount = static_cast<uint32_t>(renderOptions.clearValues.size()),
                                   .pClearValues = renderOptions.clearValues.data()};
}

vk::Extent2D Renderer::getFramebufferExtent() const
{
    return framebufferExtent;
}

void Renderer::createFramebuffers(std::vector<vk::ImageView> imageViews)
{
    framebuffers.clear();
    framebuffers.reserve(renderSource->getImageCount());
    if (renderOptions.useDepth)
    {
        createDepthImages();
        createDepthImageViews();
    }

    for (auto i = 0; i < renderSource->getImageCount(); ++i)
    {
        std::vector<vk::ImageView> attachments;
        attachments.emplace_back(imageViews.at(i));
        if (renderOptions.useDepth)
            attachments.emplace_back(getDepthImageView(i));

        framebuffers.emplace_back(
            std::make_unique<core::Framebuffer>(device, *renderPass, attachments, framebufferExtent));
    }
}

void Renderer::createDepthImages()
{
    uint32_t numImages = renderSource->getImageCount();
    auto extent = renderSource->getExtent();

    depthImages.clear();
    depthImages.reserve(numImages);

    for (auto i = 0; i < numImages; ++i)
    {
        vk::ImageCreateInfo imageInfo{.imageType = vk::ImageType::e2D,
                                      .format = vk::Format::eD32Sfloat,
                                      .extent =
                                          vk::Extent3D{.width = extent.width, .height = extent.height, .depth = 1},
                                      .mipLevels = 1,
                                      .arrayLayers = 1,
                                      .samples = vk::SampleCountFlagBits::e1,
                                      .tiling = vk::ImageTiling::eOptimal,
                                      .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                      .sharingMode = vk::SharingMode::eExclusive,
                                      .initialLayout = vk::ImageLayout::eUndefined};
        depthImages.emplace_back(device, imageInfo, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
    }
}

void Renderer::createDepthImageViews()
{
    depthImageViews.clear();
    depthImageViews.reserve(depthImages.size());
    for (const auto &image : depthImages)
    {
        depthImageViews.emplace_back(image.createImageView(vk::ImageAspectFlagBits::eDepth));
    }
}

vk::ImageView Renderer::getDepthImageView(uint32_t index)
{
    return *depthImageViews[index];
}

} // namespace vkf::rendering