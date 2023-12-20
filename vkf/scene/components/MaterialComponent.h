/// \file
/// \brief

//
// Created by Joshua Lowe on 12/18/2023.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#ifndef VULKANRENDERER_MATERIALCOMPONENT_H
#define VULKANRENDERER_MATERIALCOMPONENT_H

#include "../../core/Pipeline.h"

namespace vkf::scene
{

struct MaterialComponent
{
    MaterialComponent(core::Pipeline &pipeline) : pipeline{std::move(pipeline)}
    {
    }

    void addBuffer(const std::string &bufferName, uint32_t index)
    {
        bufferMap[bufferName] = index;
        if (index < maxSize)
        {
            indices[currentBufferCount] = index;
            currentBufferCount++;
        }
        else
        {
            // Handle error: index is out of bounds
        }
    }

    uint32_t getBufferIndex(const std::string &bufferName)
    {
        auto it = bufferMap.find(bufferName);
        if (it != bufferMap.end())
        {
            return it->second;
        }
        else
        {
            // Handle error: bufferName not found in the map
            return -1;
        }
    }

    static constexpr uint32_t maxSize{32}; // Maximum size you expect to need
    uint32_t indices[maxSize];
    core::Pipeline pipeline;
    std::unordered_map<std::string, uint32_t> bufferMap;
    uint32_t currentBufferCount{0};
};

} // namespace vkf::scene

#endif // VULKANRENDERER_MATERIALCOMPONENT_H