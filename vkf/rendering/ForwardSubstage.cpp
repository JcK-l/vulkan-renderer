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
#include "../rendering/BindlessManager.h"
#include "../scene/Scene.h"

namespace vkf::rendering
{

ForwardSubstage::ForwardSubstage(scene::Scene &scene, platform::Gui *inputGui,
                                 const rendering::BindlessManager &bindlessManager)
    : scene{scene}, gui{inputGui}, bindlessManager{bindlessManager}
{
}

void ForwardSubstage::draw(vk::raii::CommandBuffer *cmd)
{
    auto view = scene.getRegistry().view<scene::MaterialComponent>();

    cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, bindlessManager.getPipelineLayout(), 0,
                            {bindlessManager.getDescriptorSet()}, {});

    vk::Viewport viewport{.x = 0.0f,
                          .y = 0.0f,
                          .width = static_cast<float>(gui->getExtent().width),
                          .height = static_cast<float>(gui->getExtent().height),
                          .minDepth = 0.0f,
                          .maxDepth = 1.0f};
    cmd->setViewport(0, {viewport});

    vk::Rect2D scissor{.offset = {0, 0}, .extent = gui->getExtent()};
    cmd->setScissor(0, {scissor});

    for (auto entity : view)
    {
        auto &materialComponent = view.get<scene::MaterialComponent>(entity);
        core::Pipeline &pipeline = materialComponent.pipeline;

        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.getHandle());

        cmd->pushConstants<uint32_t>(bindlessManager.getPipelineLayout(), vk::ShaderStageFlagBits::eAll, 0,
                                     materialComponent.indices);

        cmd->draw(36, 1, 0, 0);
    }
}

} // namespace vkf::rendering