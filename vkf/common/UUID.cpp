/// \file
/// \brief

//
// Created by Joshua Lowe on 1/13/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "UUID.h"
#include <random>

namespace vkf
{

static std::random_device rd;
static std::mt19937_64 gen(rd());
static std::uniform_int_distribution<uint64_t> dis;

UUID::UUID() : uuid{dis(gen)}
{
}

bool UUID::operator==(const UUID &other) const
{
    return this->uuid == other.uuid;
}

} // namespace vkf