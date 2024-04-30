/// \file
/// \brief

//
// Created by Joshua Lowe on 1/28/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "PoleComponent.h"
#include "../Camera.h"
#include "glm/gtc/type_ptr.hpp"
#include <imgui.h>

namespace vkf::scene
{

PoleComponent::PoleComponent(scene::Camera *camera) : camera{camera}
{
    glm::vec3 offsetDirection = 0.8f * camera->getXAxis();
    poleData.offsetDirection = offsetDirection;
    poleData.cameraPosition = camera->getPosition();
}

void PoleComponent::updateGui()
{
    ImGui::Text("Pole:");
    ImGui::Spacing();
    std::string colorLabel = "Color Picker##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    ImGui::ColorEdit4(colorLabel.c_str(), glm::value_ptr(poleData.geometryColor));
    ImGui::Spacing();
    std::string zParamsLabel = "Pole Z Params##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    ImGui::DragFloat2(zParamsLabel.c_str(), glm::value_ptr(poleData.pToWorldZParams), 0.01f, -20.f, 20.0f);
    ImGui::Spacing();

    std::string isTubeLabel = "isTube##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    if (ImGui::Checkbox(isTubeLabel.c_str(), &isTube))
    {
        hasChanged = true;
    }

    if (isTube)
    {
        std::string tubeRadiusLabel = "Tube Radius##" + std::to_string(reinterpret_cast<uintptr_t>(this));
        ImGui::DragFloat(tubeRadiusLabel.c_str(), &poleData.tubeRadius, 0.01f, 0.0f, 2.0f);
        ImGui::Spacing();
        std::string segmentLabel = "End Segment Offset##" + std::to_string(reinterpret_cast<uintptr_t>(this));
        ImGui::DragFloat(segmentLabel.c_str(), &poleData.endSegmentOffset, 0.01f, 0.0f, 2.0f);
        ImGui::Spacing();
    }
}

void PoleComponent::updateData()
{
    glm::vec3 offsetDirection = 0.8f * camera->getXAxis();
    poleData.offsetDirection = offsetDirection;
    poleData.cameraPosition = camera->getPosition();
}

} // namespace vkf::scene