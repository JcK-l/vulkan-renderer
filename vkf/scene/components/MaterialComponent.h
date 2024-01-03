////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file MaterialComponent.h
/// \brief This file declares the MaterialComponent struct which is used for managing material data in a scene.
///
/// The MaterialComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage
/// material data. It also provides methods to add buffers, get buffer index, and handle errors.
///
/// \author Joshua Lowe
/// \date 12/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_MATERIALCOMPONENT_H
#define VULKANRENDERER_MATERIALCOMPONENT_H

namespace vkf::core // Forward declarations
{
class Pipeline;
} // namespace vkf::core

namespace vkf::scene
{

///
/// \struct MaterialComponent
/// \brief Struct for managing material data in a scene.
///
/// This struct provides functionality to store and manage material data. It contains a core::Pipeline member to store
/// the pipeline data, a map to store buffer names and their indices, and an array to store indices.
///
struct MaterialComponent
{
    ///
    /// \brief Constructor that takes a core::Pipeline as parameter.
    ///
    /// This constructor initializes the pipeline member with the provided pipeline.
    ///
    explicit MaterialComponent(core::Pipeline *pipeline) : pipeline{pipeline}
    {
    }

    ///
    /// \brief Method to add a buffer.
    ///
    /// This method takes a buffer name and an index, and adds them to the buffer map. If the index is within the
    /// maximum size, it also adds the index to the indices array and increments the current buffer count.
    ///
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

    ///
    /// \brief Method to get a buffer index.
    ///
    /// This method takes a buffer name and returns its index from the buffer map. If the buffer name is not found in
    /// the map, it handles the error and returns -1.
    ///
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

    static constexpr uint32_t maxSize{32};
    uint32_t indices[maxSize];
    core::Pipeline *pipeline;
    std::unordered_map<std::string, uint32_t> bufferMap; ///< Map to store buffer names and their indices
    uint32_t currentBufferCount{0};
};

} // namespace vkf::scene

#endif // VULKANRENDERER_MATERIALCOMPONENT_H