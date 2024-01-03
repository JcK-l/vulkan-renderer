////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Entity.h
/// \brief This file implements the Entity class which is used for managing entities in a scene.
///
/// The Entity class is part of the vkf::scene namespace. It provides functionality to create, destroy, and manage
/// entities. It also provides methods to add components to an entity, get a component from an entity, get and set the
/// handle of an entity.
///
/// \author Joshua Lowe
/// \date 12/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Entity.h"
#include "../common/Log.h"
#include "components/Components.h"
#include "imgui.h"

namespace vkf::scene
{

Entity::Entity(entt::registry &registry, rendering::BindlessManager &bindlessManager)
    : bindlessManager{bindlessManager}, registry{registry}
{
}

void Entity::create(std::string tag)
{
    handle = registry.create();
    this->addComponent<scene::TagComponent>(std::move(tag));
    LOG_INFO("Entity created")
}

void Entity::destroy()
{
    registry.destroy(handle);
    handle = entt::null;
    LOG_INFO("Entity destroyed")
}

entt::entity Entity::getHandle() const
{
    return handle;
}

void Entity::setHandle(entt::entity handle)
{
    this->handle = handle;
}

void Entity::setHandle(Entity entity)
{
    this->handle = entity.getHandle();
}

void Entity::displayGui()
{
    auto &tag = this->getComponent<scene::TagComponent>();
    ImGui::Text("Selected Entity: %s", tag.tag.c_str());
}

void Entity::updateComponents()
{
}

} // namespace vkf::scene