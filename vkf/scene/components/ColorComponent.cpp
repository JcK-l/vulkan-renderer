////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file ColorComponent.cpp
/// \brief This file implements the ColorComponent struct which is used for managing color data in a scene.
///
/// The ColorComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage color
/// data.
///
/// \author Joshua Lowe
/// \date 12/17/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ColorComponent.h"
#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

void ColorComponent::displayGui()
{
    ImGui::Text("Color:");
    ImGui::Spacing();
    std::string colorLabel = "Color Picker##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    ImGui::ColorEdit4(colorLabel.c_str(), glm::value_ptr(color));
    ImGui::Spacing();
}

void ColorComponent::setColor(glm::vec4 color)
{
    this->color = color;
}

} // namespace vkf::scene