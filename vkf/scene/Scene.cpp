////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Scene.cpp
/// \brief This file defines the Scene class which is used for managing a scene.
///
/// The Scene class is part of the vkf::scene namespace. It provides an interface for interacting with a scene.
///
/// \author Joshua Lowe
/// \date 12/3/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Scene.h"
#include "../common/Log.h"
#include "../core/Pipeline.h"
#include "../core/RenderPass.h"
#include "../rendering/BindlessManager.h"
#include "Camera.h"
#include "Entity.h"
#include "prefabs/Prefab.h"
#include "prefabs/PrefabFactory.h"

namespace vkf::scene
{

Scene::Scene(const core::Device &device, rendering::BindlessManager &bindlessManager,
             const core::RenderPass &renderPass, Camera &camera)
    : device{device}, bindlessManager{bindlessManager}, renderPass{renderPass},
      sceneCamera{std::make_unique<Camera>(std::move(camera))},
      prefabFactory{std::make_unique<PrefabFactory>(device, bindlessManager, renderPass)} {LOG_INFO("Scene created")};

Scene::~Scene() = default;

void Scene::createPrefab(PrefabType type, std::string tag)
{
    auto prefabFunctions = prefabFactory->getPrefabFunctions(type);
    auto pair = prefabFunctions.prefabCreate(registry, std::move(tag), this);
    selectedPrefabUUID = pair.first;
    prefabs.emplace(selectedPrefabUUID, std::move(pair.second));
}

entt::entity Scene::getActiveEntity()
{
    if (prefabs[selectedPrefabUUID] == nullptr)
    {
        return entt::null;
    }

    return prefabs[selectedPrefabUUID]->getEntity();
}

entt::entity Scene::getLastSelectedChild()
{
    return lastSelectedChild;
}

void Scene::updateSelectedPrefabGui()
{
    prefabs[selectedPrefabUUID]->updateGui();
}

void Scene::updateSelectedPrefabComponents()
{
    prefabs[selectedPrefabUUID]->updateComponents();
}

void Scene::updateGlobalFunctions()
{
    for (auto &pair : globalFunctions)
    {
        pair.second();
    }
}

void Scene::addGlobalFunction(UUID uuid, std::function<void()> function)
{
    globalFunctions.emplace(uuid, std::move(function));
}

void Scene::removeGlobalFunction(UUID uuid)
{
    globalFunctions.erase(uuid);
}

void Scene::destroySelectedPrefab()
{
    prefabs[selectedPrefabUUID]->destroy();
}

entt::registry &Scene::getRegistry()
{
    return registry;
}

Camera *Scene::getCamera() const
{
    return sceneCamera.get();
}

UUID Scene::getSelectedPrefabUUID() const
{
    return selectedPrefabUUID;
}

void Scene::setSeletedPrefab(UUID uuid)
{
    selectedPrefabUUID = uuid;
}

void Scene::setLastSelectedChild(entt::entity entity)
{
    lastSelectedChild = entity;
}

} // namespace vkf::scene