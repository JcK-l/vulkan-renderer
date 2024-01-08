////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PrefabComponent.h
/// \brief This file declares the PrefabComponent struct which is used for managing prefab data for an entity in a
/// scene.
///
/// The PrefabComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage prefab
/// data.
///
/// \author Joshua Lowe
/// \date 1/6/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_PREFABCOMPONENT_H
#define VULKANRENDERER_PREFABCOMPONENT_H

#include "../prefabs/PrefabType.h"

namespace vkf::scene
{

///
/// \struct PrefabComponent
/// \brief Struct for managing prefab data for an entity in a scene.
///
/// This struct provides functionality to store and manage prefab data. It contains a PrefabType member to store the
/// prefab type.
///
struct PrefabComponent
{
    PrefabComponent() = default; ///< Default constructor

    ///
    /// \brief Constructor that takes a PrefabType as parameter.
    ///
    /// This constructor initializes the prefabType member with the provided prefabType.
    ///
    explicit PrefabComponent(PrefabType prefabType) : prefabType{prefabType}
    {
    }

    PrefabType prefabType;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_PREFABCOMPONENT_H