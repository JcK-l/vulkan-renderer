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
#include "../core/RenderPass.h"
#include "../core/Shader.h"
#include "../rendering/BindlessManager.h"
#include "../rendering/PipelineBuilder.h"
#include "components/Components.h"
#include "imgui.h"

namespace vkf::scene
{

Entity::Entity(entt::registry &registry) : registry{registry}
{
}

Entity &Entity::operator=(Entity &&other) noexcept
{
    if (this != &other)
    {

        // Transfer the ownership of other's resources to *this
        registry = std::move(other.registry);
        handle = std::move(other.handle);

        // Leave other in a safely destructible state
        other.handle = entt::null;
    }
    return *this;
}

entt::entity Entity::getHandle() const
{
    return handle;
}

void Entity::setHandle(entt::entity handle)
{
    this->handle = handle;
}

void Entity::create()
{
    handle = registry.create();
    LOG_INFO("Entity created")
}

void Entity::destroy()
{
    registry.destroy(handle);
    handle = entt::null;
    LOG_INFO("Entity destroyed")
}

} // namespace vkf::scene