////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file ColorComponent.h
/// \brief This file declares the ColorComponent struct which is used for managing color data in a scene.
///
/// The ColorComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage color
/// data.
///
/// \author Joshua Lowe
/// \date 12/17/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_COLORCOMPONENT_H
#define VULKANRENDERER_COLORCOMPONENT_H

#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vkf::scene
{

///
/// \struct ColorComponent
/// \brief Struct for managing color data in a scene.
///
/// This struct provides functionality to store and manage color data. It contains a glm::vec4 member to store the color
/// data.
///
struct ColorComponent
{
    ///
    /// \brief Constructor that takes a glm::vec4 color as parameter.
    ///
    /// This constructor initializes the color member with the provided color.
    ///
    explicit ColorComponent(glm::vec4 color) : color{color}
    {
    }

    void displayGui()
    {
        // Display color
        ImGui::ColorEdit4("Color Picker", glm::value_ptr(color));
    }

    glm::vec4 color;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_COLORCOMPONENT_H