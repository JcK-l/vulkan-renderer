////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PrefabFactory.cpp
/// \brief This file implements the PrefabFactory class, which is used to create prefabricated entities in the
/// vkf::scene namespace.
///
/// The PrefabFactory class is part of the vkf::scene namespace. It provides an interface for creating prefabricated
/// entities, such as Cubes and Triangles. These entities are used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrefabFactory.h"
#include "../../common/Log.h"
#include "../../core/Device.h"
#include "../../core/RenderPass.h"
#include "../../core/Shader.h"
#include "../../rendering/BindlessManager.h"
#include "../../rendering/PipelineBuilder.h"
#include "Prefab.h"

namespace vkf::scene
{

PrefabFactory::PrefabFactory(const core::Device &device, rendering::BindlessManager &bindlessManager,
                             const core::RenderPass &renderPass)
    : device{device}, bindlessManager{bindlessManager}, renderPass{renderPass}
{
    createPipelines();
}

PrefabFactory::~PrefabFactory() = default;

void PrefabFactory::createPipelines()
{
    for (const auto &pair : PrefabTypeManager::prefabNames)
    {
        LOG_INFO("Creating pipelines for prefab type: {}", pair.second);
        auto prefabFunctions = prefabTypeManager.getPrefabFunctions(pair.first);
        auto pipelineBuilders = prefabFunctions.pipelineBuild(device, renderPass, bindlessManager);

        std::deque<std::unique_ptr<core::Pipeline>> pipelines;

        for (auto &pipelineBuilder : pipelineBuilders)
        {
            pipelines.emplace_back(std::make_unique<core::Pipeline>(pipelineBuilder.build(device)));
        }
        pipelineMap.emplace(pair.first, std::move(pipelines));
    }
}

PrefabTypeManager::PrefabFunctions PrefabFactory::getPrefabFunctions(PrefabType type) const
{
    return prefabTypeManager.getPrefabFunctions(type);
}

} // namespace vkf::scene