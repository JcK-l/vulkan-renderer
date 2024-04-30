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

ForwardSubstage::ForwardSubstage(scene::Scene &scene, RenderSource *source,
                                 const rendering::BindlessManager &bindlessManager)
    : scene{scene}, source{source}, bindlessManager{bindlessManager}
{
}

void ForwardSubstage::draw(vk::raii::CommandBuffer *cmd)
{
    cmd->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, bindlessManager.getPipelineLayout(), 0,
                            {bindlessManager.getDescriptorSet()}, {});

    vk::Viewport viewport{.x = 0.0f,
                          .y = 0.0f,
                          .width = static_cast<float>(source->getExtent().width),
                          .height = static_cast<float>(source->getExtent().height),
                          .minDepth = 0.0f,
                          .maxDepth = 1.0f};
    cmd->setViewport(0, {viewport});

    vk::Rect2D scissor{.offset = {0, 0}, .extent = source->getExtent()};
    cmd->setScissor(0, {scissor});

    auto view = scene.getRegistry().view<scene::MeshComponent, scene::MaterialComponent>();

    for (auto entity : view)
    {
        auto &meshComp = view.get<scene::MeshComponent>(entity);
        if (!meshComp.shouldDraw)
        {
            continue;
        }
        auto &materialComp = view.get<scene::MaterialComponent>(entity);

        core::Pipeline *pipeline = materialComp.currentPipeline;

        cmd->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline->getHandle());

        cmd->pushConstants<uint32_t>(bindlessManager.getPipelineLayout(), vk::ShaderStageFlagBits::eAll, 0,
                                     materialComp.indices);
        cmd->bindVertexBuffers(0, {meshComp.vertexBuffer->getBuffer()}, {0});

        if (!meshComp.multiDraw)
        {
            cmd->draw(meshComp.numVertices, 1, 0, 0);
        }
        else
        {
            //        cmd->drawMultiEXT(meshComp.multiDrawInfos, 1, 0);
            for (int i = 0; i < meshComp.startIndices.size(); ++i)
            {
                cmd->draw(meshComp.vertexCounts[i], 1, meshComp.startIndices[i], 0);
            }
        }
    }
}
std::string ForwardSubstage::getType()
{
    return "Forward";
}

} // namespace vkf::rendering