////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file IdComponent.h
/// \brief This file declares the IdComponent struct which is used for managing UUIDs.
///
/// The IdComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage UUIDS.
///
/// \author Joshua Lowe
/// \date 1/6/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../common/UUID.h"

namespace vkf::scene
{

///
/// \struct IdComponent
/// \brief Struct for managing UUIDs.
///
/// This struct provides functionality to store and manage UUIDs. It contains a UUID member.
///
struct IdComponent
{
    IdComponent() = default; ///< Default constructor

    ///
    /// \brief Constructor that takes a UUID as parameter.
    ///
    /// This constructor initializes the UUID member with the provided UUID.
    ///
    explicit IdComponent(UUID uuid) : uuid{uuid}
    {
    }

    UUID uuid;
};

} // namespace vkf::scene