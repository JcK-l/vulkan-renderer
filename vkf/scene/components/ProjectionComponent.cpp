/// \file
/// \brief

//
// Created by Joshua Lowe on 1/22/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "ProjectionComponent.h"
#include <imgui.h>

namespace vkf::scene
{

void ProjectionComponent::updateGui()
{
    int optionInt = static_cast<int>(mapProjection);

    ImGui::Text("Choose a Projection:");
    if (projectionFlags & ProjectionFlags::CYLINDRICAL_ENABLE)
    {
        ImGui::RadioButton("Cylindrical", &optionInt, static_cast<int>(ProjectionType::CYLINDRICAL));
        ImGui::SameLine();
    }
    if (projectionFlags & ProjectionFlags::PROJ_LIBRARY_ENABLE)
    {
        ImGui::RadioButton("Proj Library", &optionInt, static_cast<int>(ProjectionType::PROJ_LIBRARY));
        ImGui::SameLine();
    }
    if (projectionFlags & ProjectionFlags::ROTATEDLATLON_ENABLE)
    {
        ImGui::RadioButton("RotatedLatLon", &optionInt, static_cast<int>(ProjectionType::ROTATEDLATLON));
    }

    mapProjection = static_cast<ProjectionType>(optionInt);

    switch (mapProjection)
    {
    case ProjectionType::CYLINDRICAL:
        break;
    case ProjectionType::PROJ_LIBRARY:
        if (ImGui::InputText("Proj String", projLibraryString.data(), projLibraryString.size() + 50))
        {
        }
        break;
    case ProjectionType::ROTATEDLATLON:
        ImGui::SliderFloat("Pole Longitudes", &rotatedNorthPoleLongitude, -179.9f, 179.9f);
        ImGui::SliderFloat("Pole Latitudes", &rotatedNorthPoleLatitude, -89.9f, 89.9f);
        break;
    }

    ImGui::Spacing();
}

} // namespace vkf::scene