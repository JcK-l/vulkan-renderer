////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Utility.cpp
/// \brief This file implements utility functions for the Vulkan Framework (vkf).
///
/// The functions in this file are part of the vkf namespace. They provide utility operations that are used throughout
/// the Vulkan Framework. Currently, it includes a function to convert Vulkan queue flags to a string representation.
///
/// \author Joshua Lowe
/// \date 11/16/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Utility.h"
#include "../scene/prefabs/PrefabTypeManager.h"
#include "GeometryHandling.h"
#include "Log.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <regex>

namespace vkf
{

std::string getQueueFlagsString(vk::QueueFlags flags)
{
    std::string capabilities;
    if (flags & vk::QueueFlagBits::eGraphics)
    {
        capabilities += "Graphics, ";
    }
    if (flags & vk::QueueFlagBits::eCompute)
    {
        capabilities += "Compute, ";
    }
    if (flags & vk::QueueFlagBits::eTransfer)
    {
        capabilities += "Transfer, ";
    }
    if (flags & vk::QueueFlagBits::eSparseBinding)
    {
        capabilities += "Sparse Binding, ";
    }
    if (flags & vk::QueueFlagBits::eProtected)
    {
        capabilities += "Protected, ";
    }

    if (capabilities.empty())
    {
        capabilities = "None";
    }
    else
    {
        // Remove the trailing comma and space
        capabilities.pop_back();
        capabilities.pop_back();
    }

    return capabilities;
}

glm::mat4 recompose(glm::vec3 translate, glm::quat rotation, glm::vec3 scale)
{
    auto matrix = glm::mat4(1.0f);

    matrix *= glm::translate(translate);
    matrix *= glm::mat4_cast(rotation);
    matrix *= glm::scale(scale);

    return matrix;
}

glm::vec3 calculateSrgbColor(glm::vec3 linearColor)
{
    glm::vec3 srgbColor;
    for (int i = 0; i < 3; ++i)
    {
        if (linearColor[i] <= 0.0031308f)
        {
            srgbColor[i] = linearColor[i] * 12.92f;
        }
        else
        {
            srgbColor[i] = 1.055f * std::pow(linearColor[i], 1.0f / 2.4f) - 0.055f;
        }
    }
    return srgbColor;
}

glm::vec3 calculateLinearColor(glm::vec3 srgbColor)
{
    glm::vec3 linearColor;
    for (int i = 0; i < 3; ++i)
    {
        if (srgbColor[i] <= 0.04045f)
        {
            linearColor[i] = srgbColor[i] / 12.92f;
        }
        else
        {
            linearColor[i] = std::pow((srgbColor[i] + 0.055f) / 1.055f, 2.4f);
        }
    }
    return linearColor;
}

std::vector<float> createRange(float begin, float end, float step)
{
    std::vector<float> range;
    int rangeLength = end - begin;
    int numSteps = static_cast<int>(rangeLength / step);
    for (int i = 0; i <= numSteps; ++i)
    {
        float value = begin + i * step;
        range.emplace_back(value);
    }
    return range;
}

} // namespace vkf

namespace Met3D
{

void translate(std::vector<Met3D::PointF> &polygon, float dx, float dy)
{
    for (Met3D::PointF &point : polygon)
    {
        point.x += dx;
        point.y += dy;
    }
}
} // namespace Met3D