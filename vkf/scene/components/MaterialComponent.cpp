////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file MaterialComponent.cpp
/// \brief This file implements the MaterialComponent struct which is used for managing material data in a scene.
///
/// The MaterialComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage
/// material data. It also provides methods to add buffers, get buffer index, and handle errors.
///
/// \author Joshua Lowe
/// \date 12/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MaterialComponent.h"

namespace vkf::scene
{

void MaterialComponent::addUniform(const std::string &uniformName, uint32_t index)
{
    uniformMap[uniformName] = index;
    indices[currentUniformCount] = index;
    currentUniformCount++;
}

uint32_t MaterialComponent::getUniformIndex(const std::string &uniformName)
{
    auto it = uniformMap.find(uniformName);
    if (it != uniformMap.end())
    {
        return it->second;
    }
    else
    {
        // Handle error: uniformName not found in the map
        return -1;
    }
}

} // namespace vkf::scene