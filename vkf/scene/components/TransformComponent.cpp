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
#include "../Camera.h"
#include "ImGuizmo.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

TransformComponent::TransformComponent(Camera *camera, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    : camera{camera}, translation{translation}, rotation{rotation}, scale{scale}, modelMatrix{glm::mat4(1.0f)}
{
    ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(translation), glm::value_ptr(rotation),
                                            glm::value_ptr(scale), glm::value_ptr(modelMatrix));
}

void TransformComponent::updateGui()
{
    ImGui::Text("Transform:");
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("- Guizmo -\n"
                          "R to rotate, T to translate, S to scale\n"
                          "Hold Ctrl to snap");
    }
    ImGui::Spacing();

    // Display translation
    std::string translationLabel = "Position##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::DragFloat3(translationLabel.c_str(), glm::value_ptr(translation), 1.f))
    {
        modelHasChanged = true;
    }

    // Display rotation
    std::string rotationLabel = "Rotation##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::DragFloat3(rotationLabel.c_str(), glm::value_ptr(rotation), 1.f))
    {
        modelHasChanged = true;
    };

    // Display scale
    static bool linkValues = false;
    std::string scaleLabel = "Scale##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    auto newScale = scale;
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
        scale = newScale;
        modelHasChanged = true;
    }

    if (modelHasChanged)
    {
        //        modelMatrix = recompose(translation, glm::quat(rotation), scale);
        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(translation), glm::value_ptr(rotation),
                                                glm::value_ptr(scale), glm::value_ptr(modelMatrix));
        modelHasChanged = false;
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