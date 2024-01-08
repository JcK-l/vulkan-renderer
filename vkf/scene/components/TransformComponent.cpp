////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file TransformComponent.cpp
/// \brief This file implements the TransformComponent struct which is used for managing transformation data for an
/// entity in a scene.
///
/// The TransformComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage
/// transformation data. It contains glm::vec3 members to store the position, rotation, and scale data.
///
/// \author Joshua Lowe
/// \date 12/17/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TransformComponent.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

void TransformComponent::displayGui()
{
    ImGui::Text("Transform:");
    ImGui::Spacing();

    // Display position
    std::string positionLabel = "Position##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    ImGui::DragFloat3(positionLabel.c_str(), glm::value_ptr(position), 0.01f);

    // Display rotation
    std::string rotationLabel = "Rotation##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    ImGui::DragFloat3(rotationLabel.c_str(), glm::value_ptr(rotation), 0.01f);

    // Display scale
    static bool linkValues = false;
    glm::vec3 newScale = scale; // Capture the state of tc.scale

    std::string scaleLabel = "Scale##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::DragFloat3(scaleLabel.c_str(), glm::value_ptr(newScale), 0.01f))
    {
        if (linkValues)
        {
            for (int i = 0; i < 3; ++i)
            {
                if (newScale[i] != scale[i]) // Compare newScale with tc.scale
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        if (i != j)
                        {
                            newScale[j] = newScale[i]; // Set the other two values to the changed value
                        }
                    }
                    break; // No need to check the other values once we found the changed one
                }
            }
        }
        scale = newScale; // Update tc.scale with the new values
    }

    ImGui::SameLine();

    std::string linkLabel = "Link##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (linkValues)
    {
        // Change the button color to highlight it when linking is active
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 120, 215, 255)); // Blue color

        if (ImGui::Button(linkLabel.c_str()))
        {
            linkValues = !linkValues;
        }

        // Reset the button color back to default when done
        ImGui::PopStyleColor();
    }
    else
    {
        if (ImGui::Button(linkLabel.c_str()))
        {
            linkValues = !linkValues;
        }
    }

    ImGui::Spacing();
}

} // namespace vkf::scene