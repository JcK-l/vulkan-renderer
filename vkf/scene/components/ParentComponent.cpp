////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file ParentComponent.cpp
/// \brief This file implements the ParentComponent struct which is used for managing parent-child relationships for
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

#include "ParentComponent.h"

namespace vkf::scene
{

void ParentComponent::addChild(Entity child)
{
    children.emplace_back(std::make_shared<Entity>(std::move(child)));
}

} // namespace vkf::scene