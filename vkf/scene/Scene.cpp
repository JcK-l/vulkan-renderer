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

namespace vkf::scene
{

Scene::Scene()
{
}

entt::entity Scene::createEntity(const std::string &name)
{
    entt::entity entity = registry.create();
    registry.emplace<TagComponent>(entity, name);
    return entity;
}

entt::registry &Scene::getRegistry()
{
    return registry;
}

} // namespace vkf::scene