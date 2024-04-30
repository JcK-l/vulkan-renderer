/// \file
/// \brief

//
// Created by Joshua Lowe on 1/19/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#pragma once

namespace vkf::scene
{

struct GraticuleComponent
{
    void updateGui();

    std::array<float, 3> graticuleLongitudes{-180, 180, 20};
    std::array<float, 3> graticuleLatitudes{-90, 90, 10};

    float graticuleSpacingLongitude{1.};
    float graticuleSpacingLatitude{1.};

    bool hasNewGraticule{false};
};

} // namespace vkf::scene