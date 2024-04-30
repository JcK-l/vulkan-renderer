////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file MaterialComponent.h
/// \brief This file declares the MaterialComponent struct which is used for managing material data in a scene.
///
/// The MaterialComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage
/// material data. It also provides methods to add resource, get resouce index, and handle errors.
///
/// \author Joshua Lowe
/// \date 12/18/2023
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Forward declarations
#include "../../core/CoreFwd.h"

namespace vkf::scene
{

///
/// \struct MaterialComponent
/// \brief Struct for managing material data in a scene.
///
/// This struct provides functionality to store and manage material data. It contains a core::Pipeline member to store
/// the pipeline data, a map to store resource names and their indices, and an array to store indices.
///
struct MaterialComponent
{
    ///
    /// \brief Constructor that takes a core::Pipeline as parameter.
    ///
    /// This constructor initializes the pipeline member with the provided pipeline.
    ///
    explicit MaterialComponent(std::deque<core::Pipeline *> pipelines)
        : pipelines{std::move(pipelines)}, currentPipeline{this->pipelines.front()}
    {
    }

    ///
    /// \brief Method to add a resource.
    ///
    /// This method takes a resource name and an index, and adds them to the resource map.
    /// It also adds the index to the indices array and increments the current resource count.
    ///
    void addResource(const std::string &resourceName, uint32_t index);

    ///
    /// \brief Method to get a resource index.
    ///
    /// This method takes a resource name and returns its index from the resource map. If the resource name is not found
    /// in the map, it handles the error and returns -1.
    ///
    uint32_t getResourceIndex(const std::string &resourceName);

    void setPipeline(uint32_t index);

    static constexpr uint32_t maxSize{32};
    std::array<uint32_t, maxSize> indices;
    std::deque<core::Pipeline *> pipelines;
    core::Pipeline *currentPipeline;
    std::unordered_map<std::string, uint32_t> resourceMap; ///< Map to store resource names and their indices

    uint32_t currentResourceCount{0};
};

} // namespace vkf::scene