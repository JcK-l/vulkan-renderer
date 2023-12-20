/// \file
/// \brief

//
// Created by Joshua Lowe on 12/17/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_COLORCOMPONENT_H
#define VULKANRENDERER_COLORCOMPONENT_H

#include <glm/glm.hpp>

namespace vkf::scene
{
struct ColorComponent
{
    explicit ColorComponent(glm::vec4 color) : color{color}
    {
    }

    glm::vec4 color;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_COLORCOMPONENT_H