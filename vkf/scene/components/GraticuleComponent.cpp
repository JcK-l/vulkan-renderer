/// \file
/// \brief

//
// Created by Joshua Lowe on 1/19/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "GraticuleComponent.h"
#include <imgui.h>

namespace vkf::scene
{

void GraticuleComponent::updateGui()
{
    ImGui::Text("Graticule:");
    ImGui::Spacing();

    ImGui::Text("Longitudes:");
    ImGui::InputFloat3("##Longitudes", graticuleLongitudes.data());

    ImGui::Text("Latitudes:");
    ImGui::InputFloat3("##Latitudes", graticuleLatitudes.data());

    ImGui::Spacing();

    if (ImGui::Button("Load Graticule"))
    {
        hasNewGraticule = true;
    }

    ImGui::Spacing();
}

} // namespace vkf::scene
