/// \file
/// \brief

//
// Created by Joshua Lowe on 1/13/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#pragma once

#include <random>
#include <xhash>

namespace vkf
{

struct UUID
{

    UUID();                                  ///< Constructor
    UUID(const UUID &) = default;            ///< Default copy constructor
    UUID(UUID &&) noexcept = default;        ///< Default Move constructor
    UUID &operator=(const UUID &) = default; ///< Default copy assignment operator
    UUID &operator=(UUID &&) = default;      ///< Default move assignment operator
    ~UUID() = default;                       ///< Default Destructor

    bool operator==(const UUID &) const;

    uint64_t uuid;
};

} // namespace vkf

namespace std
{

template <> struct hash<vkf::UUID>
{
    size_t operator()(const vkf::UUID &uuid) const
    {
        return hash<uint64_t>()(uuid.uuid);
    }
};

} // namespace std