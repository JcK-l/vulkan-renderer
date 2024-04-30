/// \file
/// \brief

//
// Created by Joshua Lowe on 1/13/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//
#include "PrefabTypeManager.h"
#include "../../rendering/PipelineBuilder.h"
#include "BasemapActor.h"
#include "Cube.h"
#include "GraticuleActor.h"
#include "PoleActor.h"
#include "PrefabFactory.h"
#include "Texture2D.h"

namespace vkf::scene
{

PrefabTypeManager::PrefabTypeManager(PrefabFactory &prefabFactory)
{
    for (const auto &pair : prefabNames)
    {
        auto type = pair.first;
        PrefabFunctions functions;

        switch (type)
        {
        case PrefabType::Cube:
            functions.prefabCreate = [&](entt::registry &registry, std::string tag, Scene *scene) {
                return prefabFactory.createPrefab<Cube>(registry, std::move(tag), scene);
            };
            functions.pipelineBuild = [&](const core::Device &device, const core::RenderPass &renderPass,
                                          rendering::BindlessManager &bindlessManager) {
                return Cube::getPipelineBuilders(device, renderPass, bindlessManager);
            };
            break;
        case PrefabType::Texture2D:
            functions.prefabCreate = [&](entt::registry &registry, std::string tag, Scene *scene) {
                return prefabFactory.createPrefab<Texture2D>(registry, std::move(tag), scene);
            };
            functions.pipelineBuild = [&](const core::Device &device, const core::RenderPass &renderPass,
                                          rendering::BindlessManager &bindlessManager) {
                return Texture2D::getPipelineBuilders(device, renderPass, bindlessManager);
            };
            break;
        case PrefabType::BasemapActor:
            functions.prefabCreate = [&](entt::registry &registry, std::string tag, Scene *scene) {
                return prefabFactory.createPrefab<BasemapActor>(registry, std::move(tag), scene);
            };
            functions.pipelineBuild = [&](const core::Device &device, const core::RenderPass &renderPass,
                                          rendering::BindlessManager &bindlessManager) {
                return BasemapActor::getPipelineBuilders(device, renderPass, bindlessManager);
            };
            break;
        case PrefabType::GraticuleActor:
            functions.prefabCreate = [&](entt::registry &registry, std::string tag, Scene *scene) {
                return prefabFactory.createPrefab<GraticuleActor>(registry, std::move(tag), scene);
            };
            functions.pipelineBuild = [&](const core::Device &device, const core::RenderPass &renderPass,
                                          rendering::BindlessManager &bindlessManager) {
                return GraticuleActor::getPipelineBuilders(device, renderPass, bindlessManager);
            };
            break;
        case PrefabType::PoleActor:
            functions.prefabCreate = [&](entt::registry &registry, std::string tag, Scene *scene) {
                return prefabFactory.createPrefab<PoleActor>(registry, std::move(tag), scene);
            };
            functions.pipelineBuild = [&](const core::Device &device, const core::RenderPass &renderPass,
                                          rendering::BindlessManager &bindlessManager) {
                return PoleActor::getPipelineBuilders(device, renderPass, bindlessManager);
            };
            break;
        }

        prefabFunctionsMap[type] = functions;
    }
}

PrefabTypeManager::PrefabFunctions PrefabTypeManager::getPrefabFunctions(PrefabType type) const
{
    return prefabFunctionsMap.at(type);
}

std::unordered_map<PrefabType, std::string> PrefabTypeManager::prefabNames = {
    {PrefabType::Cube, "Cube"},
    {PrefabType::Texture2D, "Texture2D"},
    {PrefabType::BasemapActor, "BasemapActor"},
    {PrefabType::GraticuleActor, "GraticuleActor"},
    {PrefabType::PoleActor, "PoleActor"},
};

} // namespace vkf::scene