////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file TagComponent.h
/// \brief This file declares the TagComponent struct which is used for managing tag data for an entity in a scene.
///
/// The TagComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage tag data.
///
/// \author Joshua Lowe
/// \date 12/17/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace vkf::scene
{

///
/// \struct TagComponent
/// \brief Struct for managing tag data for an entity in a scene.
///
/// This struct provides functionality to store and manage tag data. It contains a string member to store the tag data.
///
struct TagComponent
{
    ///
    /// \brief Constructor that takes a string tag as parameter.
    ///
    /// This constructor initializes the tag member with the provided tag.
    ///
    explicit TagComponent(std::string tag) : tag{std::move(tag)}
    {
    }

    void updateGui();

    std::string tag;
};

} // namespace vkf::scene