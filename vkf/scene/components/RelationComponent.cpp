////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RelationComponent.cpp
/// \brief This file implements the RelationComponent struct which is used for managing parent-child relationships for
/// entities in a scene.
///
/// The RelationComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage
/// child entities of a parent entity.
///
/// \author Joshua Lowe
/// \date 1/6/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RelationComponent.h"
#include "IdComponent.h"

namespace vkf::scene
{

RelationComponent::RelationComponent(entt::entity parent) : parent{parent}
{
}

void RelationComponent::addChild(Entity child)
{
    auto &idComp = child.getComponent<scene::IdComponent>();
    children.emplace(idComp.uuid, std::make_shared<Entity>(std::move(child)));
    hasChildren = true;
}

void RelationComponent::removeChild(UUID uuid)
{
    children.erase(uuid);
    if (children.empty())
    {
        hasChildren = false;
    }
}

} // namespace vkf::scene