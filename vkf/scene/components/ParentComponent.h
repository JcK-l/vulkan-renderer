////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file ParentComponent.h
/// \brief This file declares the ParentComponent struct which is used for managing parent-child relationships for
/// entities in a scene.
///
/// The ParentComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage child
/// entities of a parent entity.
///
/// \author Joshua Lowe
/// \date 1/6/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_PARENTCOMPONENT_H
#define VULKANRENDERER_PARENTCOMPONENT_H

#include "../Entity.h"

namespace vkf::scene
{

///
/// \struct ParentComponent
/// \brief Struct for managing parent-child relationships for entities in a scene.
///
/// This struct provides functionality to store and manage child entities of a parent entity. It contains a vector of
/// shared pointers to Entity objects to store the child entities.
///
struct ParentComponent
{
    ParentComponent() = default; ///< Default constructor

    ///
    /// \brief Method to add a child entity to the parent.
    ///
    /// This method takes an Entity object as parameter and adds it to the vector of child entities.
    ///
    void addChild(Entity child);

    std::vector<std::shared_ptr<Entity>> children;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_PARENTCOMPONENT_H