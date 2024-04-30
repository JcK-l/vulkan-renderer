/// \file
/// \brief

//
// Created by Joshua Lowe on 1/22/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#pragma once

namespace Met3D // Forward declaration
{
struct RectF;
} // namespace Met3D

namespace vkf::scene
{

struct BoundingBoxComponent
{

    std::vector<float> getMeshData();
    Met3D::RectF getRectF();

    void updateGui();

    std::array<float, 4> bbox{-180., -90., 180., 90.};
    bool hasNewBbox{false};
    bool isInput{false};
};

} // namespace vkf::scene