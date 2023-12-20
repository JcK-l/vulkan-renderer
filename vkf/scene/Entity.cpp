/// \file
/// \brief

//
// Created by Joshua Lowe on 12/18/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Entity.h"

namespace vkf::scene
{

Entity::Entity(entt::registry &registry) : registry{registry}
{
}

void Entity::create()
{
    handle = registry.create();
}

void Entity::destroy()
{
    registry.destroy(handle);
    handle = entt::null;
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

} // namespace vkf::scene