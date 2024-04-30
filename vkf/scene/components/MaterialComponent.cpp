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

void MaterialComponent::addResource(const std::string &resourceName, uint32_t index)
{
    resourceMap[resourceName] = index;
    indices[currentResourceCount] = index;
    currentResourceCount++;
}

uint32_t MaterialComponent::getResourceIndex(const std::string &resourceName)
{
    auto it = resourceMap.find(resourceName);
    if (it != resourceMap.end())
    {
        return it->second;
    }
    else
    {
        // Handle error: uniformName not found in the map
        return -1;
    }
}

void MaterialComponent::setPipeline(uint32_t index)
{
    currentPipeline = pipelines.at(index);
}

} // namespace vkf::scene