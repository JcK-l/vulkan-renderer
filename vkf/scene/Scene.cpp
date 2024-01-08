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
      prefabFactory{std::make_unique<PrefabFactory>(device, bindlessManager, renderPass, sceneCamera.get())} {
          LOG_INFO("Scene created")};

Scene::~Scene() = default;

void Scene::createPrefab(PrefabType type, std::string tag)
{
    switch (type)
    {
    case PrefabType::Cube:
        selectedPrefab = prefabFactory->createPrefab<Cube>(registry, std::move(tag));
        break;
    case PrefabType::Texture2D:
        selectedPrefab = prefabFactory->createPrefab<Texture2D>(registry, std::move(tag));
        break;
    }
}

void Scene::setActiveEntity(entt::entity entity)
{
    selectedPrefab->setEntity(entity);
}

entt::entity Scene::getActiveEntity()
{
    if (selectedPrefab == nullptr)
    {
        return entt::null;
    }

    return selectedPrefab->getEntity().getHandle();
}

void Scene::changeSelectedPrefabType(PrefabType type)
{
    switch (type)
    {
    case PrefabType::Cube:
        selectedPrefab = std::make_unique<Cube>(registry, bindlessManager, std::move(selectedPrefab->getEntity()));
        break;
    case PrefabType::Texture2D:
        selectedPrefab = std::make_unique<Texture2D>(registry, bindlessManager, std::move(selectedPrefab->getEntity()));
        break;
    }
}

void Scene::displaySelectedPrefabGui()
{
    selectedPrefab->displayGui();
}

void Scene::updateSelectedPrefabComponents()
{
    selectedPrefab->updateComponents();
}

void Scene::destroySelectedPrefab()
{
    selectedPrefab->destroy();
}

entt::registry &Scene::getRegistry()
{
    return registry;
}

Camera *Scene::getCamera() const
{
    return sceneCamera.get();
}

} // namespace vkf::scene