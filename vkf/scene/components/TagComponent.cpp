////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file TagComponent.cpp
/// \brief This file implements the TagComponent struct which is used for managing tag data for an entity in a scene.
///
/// The TagComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage tag data.
///
/// \author Joshua Lowe
/// \date 12/17/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TagComponent.h"
#include "imgui.h"

namespace vkf::scene
{

void TagComponent::displayGui()
{
    ImGui::Separator();
    ImGui::Text("Selected Entity: %s", tag.c_str());
    ImGui::Separator();
    ImGui::Spacing();
}

} // namespace vkf::scene