////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file ForwardSubstage.cpp
/// \brief This file implements the ForwardSubstage class which is used for managing Vulkan rendering operations.
///
/// The ForwardSubstage class is part of the vkf::rendering namespace. It provides an interface for interacting with
/// Vulkan rendering operations.
///
/// \author Joshua Lowe
/// \date 12/1/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForwardSubstage.h"
#include "../core/Pipeline.h"
#include "../platform/Gui.h"

namespace vkf::rendering
{

ForwardSubstage::ForwardSubstage(const core::Pipeline &inputPipeline, platform::Gui *inputGui)
    : pipeline{inputPipeline}, gui{inputGui}
{
}

void ForwardSubstage::draw(vk::raii::CommandBuffer *cmd)
{
    cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.getHandle());

    vk::Viewport viewport{.x = 0.0f,
                          .y = 0.0f,
                          .width = static_cast<float>(gui->getExtent().width),
                          .height = static_cast<float>(gui->getExtent().height),
                          .minDepth = 0.0f,
                          .maxDepth = 1.0f};
    cmd->setViewport(0, {viewport});

    vk::Rect2D scissor{.offset = {0, 0}, .extent = gui->getExtent()};
    cmd->setScissor(0, {scissor});

    cmd->draw(3, 1, 0, 0);
}

} // namespace vkf::rendering