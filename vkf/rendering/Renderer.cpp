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

    for (auto i = 0u; i < renderOptions.numSubpasses; ++i)
    {
        subpassDescriptions[i].colorAttachmentCount = 1;
        subpassDescriptions[i].pColorAttachments = &attachmentRefs[i];
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
    renderSubstages.emplace_back(std::move(renderSubstage));
}

void Renderer::setFrameData(std::vector<FrameData *> frameData)
{
    this->frameData = std::move(frameData);
}

void Renderer::syncFrameData()
{
    for (auto i = 0u; i < frameData.size(); ++i)
    {
        device.getHandle().waitForFences(frameData[i]->getFences(), VK_TRUE, std::numeric_limits<uint64_t>::max());
    }
}

void Renderer::updateFramebuffers()
{

    framebufferExtent = renderSource->getExtent();
    createFramebuffers(renderSource->getImageViews());
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
                                   .clearValueCount = 1,
                                   .pClearValues = &renderOptions.clearValue};
}

vk::Extent2D Renderer::getFramebufferExtent() const
{
    return framebufferExtent;
}

void Renderer::createFramebuffers(std::vector<vk::ImageView> imageViews)
{
    framebuffers.clear();
    framebuffers.reserve(renderSource->getImageCount());
    for (auto i = 0; i < renderSource->getImageCount(); ++i)
    {
        std::vector<vk::ImageView> attachments;
        attachments.emplace_back(imageViews.at(i));

        framebuffers.emplace_back(
            std::make_unique<core::Framebuffer>(device, *renderPass, attachments, framebufferExtent));
    }
    LOG_INFO("Created Framebuffer x{}", renderSource->getImageCount())
}

} // namespace vkf::rendering