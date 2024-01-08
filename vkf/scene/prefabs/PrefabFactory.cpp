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
#include "../../common/Utility.h"
#include "../../core/Device.h"
#include "../../core/RenderPass.h"
#include "../../core/Shader.h"
#include "../../rendering/BindlessManager.h"
#include "../../rendering/PipelineBuilder.h"
#include "Prefab.h"

namespace vkf::scene
{

PrefabFactory::PrefabFactory(const core::Device &device, rendering::BindlessManager &bindlessManager,
                             const core::RenderPass &renderPass, Camera *camera)
    : device{device}, bindlessManager{bindlessManager}, renderPass{renderPass}, camera{camera}
{
    pipelineBuilderFunctionMap.emplace(PrefabType::Cube, &Cube::getPipelineBuilder);
    pipelineBuilderFunctionMap.emplace(PrefabType::Texture2D, &Texture2D::getPipelineBuilder);
    createPipelines();
}

PrefabFactory::~PrefabFactory() = default;

void PrefabFactory::createPipelines()
{
    rendering::PipelineBuilder pipelineBuilder;

    for (const auto &pair : pipelineBuilderFunctionMap)
    {
        LOG_INFO("Creating pipeline for prefab type: {}", getPrefabTypeString(pair.first))
        pipelineBuilder = pair.second(device, renderPass, bindlessManager);
        pipelines.emplace(pair.first, std::make_unique<core::Pipeline>(pipelineBuilder.build(device)));
    }
}

} // namespace vkf::scene