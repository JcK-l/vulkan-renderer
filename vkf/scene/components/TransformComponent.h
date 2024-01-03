////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file TransformComponent.h
/// \brief This file declares the TransformComponent struct which is used for managing transformation data for an entity
/// in a scene.
///
/// The TransformComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage
/// transformation data. It contains glm::vec3 members to store the position, rotation, and scale data.
///
/// \author Joshua Lowe
/// \date 12/17/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_TRANSFORMCOMPONENT_H
#define VULKANRENDERER_TRANSFORMCOMPONENT_H

#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

///
/// \struct TransformComponent
/// \brief Struct for managing transformation data for an entity in a scene.
///
/// This struct provides functionality to store and manage transformation data. It contains glm::vec3 members to store
/// the position, rotation, and scale data.
///
struct TransformComponent
{
    ///
    /// \brief Constructor that takes glm::vec3 position, rotation, and scale as parameters.
    ///
    /// This constructor initializes the position, rotation, and scale members with the provided values.
    ///
    TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
        : position{position}, rotation{rotation}, scale{scale}
    {
    }

    void displayGui()
    {
        // Display position
        ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);

        // Display rotation
        ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.01f);

        // Display scale
        static bool linkValues = false;
        glm::vec3 newScale = scale; // Capture the state of tc.scale

        if (ImGui::DragFloat3("Scale", glm::value_ptr(newScale), 0.01f))
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

        if (linkValues)
        {
            // Change the button color to highlight it when linking is active
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 120, 215, 255)); // Blue color

            if (ImGui::Button("Link"))
            {
                linkValues = !linkValues;
            }

            // Reset the button color back to default when done
            ImGui::PopStyleColor();
        }
        else
        {
            if (ImGui::Button("Link"))
            {
                linkValues = !linkValues;
            }
        }
    }

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_TRANSFORMCOMPONENT_H