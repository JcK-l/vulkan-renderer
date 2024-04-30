////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file RelationshipComponent.h
/// \brief This file declares the RelationComponent struct which is used for managing parent-child relationships for
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

#pragma once

#include "../../common/UUID.h"
#include "../Entity.h"

namespace vkf::scene
{

///
/// \struct RelationComponent
/// \brief Struct for managing parent-child relationships for entities in a scene.
///
/// This struct provides functionality to store and manage child entities of a parent entity. It contains a vector of
/// shared pointers to Entity objects to store the child entities.
///
struct RelationComponent
{
    RelationComponent() = default;          ///< Default constructor
    RelationComponent(entt::entity parent); ///< Constructor

    ///
    /// \brief Method to add a child entity to the parent.
    ///
    /// This method takes an Entity object as parameter and adds it to the vector of child entities.
    ///
    void addChild(Entity child);

    void removeChild(UUID uuid);

    entt::entity parent{entt::null};
    std::unordered_map<UUID, std::shared_ptr<Entity>> children;

    bool hasChildren{false};
};

} // namespace vkf::scene