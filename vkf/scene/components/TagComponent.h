/// \file
/// \brief

//
// Created by Joshua Lowe on 12/17/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_TAGCOMPONENT_H
#define VULKANRENDERER_TAGCOMPONENT_H

namespace vkf::scene
{

///
/// \struct TagComponent
/// \brief This struct stores a tag for an entity.
///
/// It includes a string tag.
///
struct TagComponent
{
    TagComponent(std::string tag) : tag{std::move(tag)}
    {
    }

    std::string tag;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_TAGCOMPONENT_H