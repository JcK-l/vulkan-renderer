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

#include <glm/vec3.hpp>

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

    void displayGui();

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_TRANSFORMCOMPONENT_H