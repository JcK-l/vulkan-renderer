/// \file
/// \brief

//
// Created by Joshua Lowe on 11/16/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "Utility.h"

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

} // namespace vkf