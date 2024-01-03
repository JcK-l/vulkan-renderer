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
#include "../core/Buffer.h"
#include "../core/Pipeline.h"
#include "../core/RenderPass.h"
#include "../rendering/BindlessManager.h"
#include "Camera.h"
#include "Entity.h"

namespace vkf::scene
{

Scene::Scene(const core::Device &device, rendering::BindlessManager &bindlessManager,
             const core::RenderPass &renderPass, Camera &camera)
    : device{device}, bindlessManager{bindlessManager}, renderPass{renderPass},
      sceneCamera{std::make_unique<Camera>(std::move(camera))},
      prefabFactory{std::make_unique<PrefabFactory>(device, bindlessManager, renderPass, sceneCamera.get())} {
          LOG_INFO("Scene created")}

      Scene::~Scene() = default;

std::unique_ptr<Entity> Scene::createEntity(PrefabType type, std::string tag)
{
    switch (type)
    {
    case PrefabType::Cube:
        return prefabFactory->createPrefab<Cube>(registry, std::move(tag));
    case PrefabType::Triangle:
        return prefabFactory->createPrefab<Triangle>(registry, std::move(tag));
    case PrefabType::Custom:
        return prefabFactory->createCustom(registry, std::move(tag));
    }
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