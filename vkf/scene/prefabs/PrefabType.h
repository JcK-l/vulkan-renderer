////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PrefabType.h
/// \brief This file declares the PrefabType enum and the PrefabTraits struct which are used for managing prefabricated
/// entities in a scene.
///
/// The PrefabType enum and the PrefabTraits struct are part of the vkf::scene namespace.
/// They provide utility when managing prefabricated entities in a scene.
///
/// \author Joshua Lowe
/// \date 1/6/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_PREFABTYPE_H
#define VULKANRENDERER_PREFABTYPE_H

namespace vkf::scene
{

// Forward declarations
class Cube;
class Texture2D;

///
/// \enum PrefabType
/// \brief Enumerates the types of prefabricated entities that can be created.
///
/// This enum provides the types of prefabricated entities that can be created. It contains Cube and Texture2D as the
/// types of prefabricated entities.
///
enum class PrefabType
{
    Cube,
    Texture2D
};

///
/// \struct PrefabTraits
/// \brief Provides traits for prefabricated entities.
///
/// This struct provides traits for prefabricated entities. It contains a method to get the PrefabType of a
/// prefabricated entity.
///
template <typename T> struct PrefabTraits;

template <> struct PrefabTraits<Cube>
{
    ///
    /// \brief Method to get the PrefabType of a Cube.
    ///
    /// This method returns the PrefabType of a Cube.
    ///
    static PrefabType getPrefabType()
    {
        return PrefabType::Cube;
    }
};

template <> struct PrefabTraits<Texture2D>
{
    ///
    /// \brief Method to get the PrefabType of a Texture2D.
    ///
    /// This method returns the PrefabType of a Texture2D.
    ///
    static PrefabType getPrefabType()
    {
        return PrefabType::Texture2D;
    }
};

} // namespace vkf::scene

#endif // VULKANRENDERER_PREFABTYPE_H