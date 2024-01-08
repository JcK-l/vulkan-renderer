////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file SelectComponent.cpp
/// \brief This file implements the SelectComponent struct which is used for managing selection state for an entity in a
/// scene.
///
/// The SelectComponent struct is part of the vkf::scene namespace. It provides functionality to toggle and store the
/// selection state.
///
/// \author Joshua Lowe
/// \date 1/7/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SelectComponent.h"

namespace vkf::scene
{

void SelectComponent::toggleSelect()
{
    selected = !selected;
}

} // namespace vkf::scene