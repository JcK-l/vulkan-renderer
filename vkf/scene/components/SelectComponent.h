////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file SelectComponent.h
/// \brief This file declares the SelectComponent struct which is used for managing selection state for an entity in a
/// scene.
///
/// The SelectComponent struct is part of the vkf::scene namespace. It provides functionality to toggle and store the
/// selection state.
///
/// \author Joshua Lowe
/// \date 1/7/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_SELECTCOMPONENT_H
#define VULKANRENDERER_SELECTCOMPONENT_H

namespace vkf::scene
{

///
/// \struct SelectComponent
/// \brief Struct for managing selection state for an entity in a scene.
///
/// This struct provides functionality to toggle and store the selection state. It contains a boolean member to store
/// the selection state.
///
struct SelectComponent
{
    explicit SelectComponent() = default; ///< Default constructor

    ///
    /// \brief Method to toggle the selection state.
    ///
    /// This method toggles the selection state of the SelectComponent.
    ///
    void toggleSelect();

    bool selected{false};
};

} // namespace vkf::scene

#endif // VULKANRENDERER_SELECTCOMPONENT_H