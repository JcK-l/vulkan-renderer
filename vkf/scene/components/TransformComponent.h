/// \file
/// \brief

//
// Created by Joshua Lowe on 12/17/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_TRANSFORMCOMPONENT_H
#define VULKANRENDERER_TRANSFORMCOMPONENT_H

#include <glm/glm.hpp>

namespace vkf::scene
{

struct TransformComponent
{
    TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
        : position{position}, rotation{rotation}, scale{scale}
    {
    }

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_TRANSFORMCOMPONENT_H
