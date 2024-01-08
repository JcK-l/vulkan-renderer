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
    /// \brief Method to add a uniform.
    ///
    /// This method takes a uniform name and an index, and adds them to the uniform map.
    /// It also adds the index to the indices array and increments the current uniform count.
    ///
    void addUniform(const std::string &uniformName, uint32_t index);

    ///
    /// \brief Method to get a uniform index.
    ///
    /// This method takes a uniform name and returns its index from the uniform map. If the uniform name is not found in
    /// the map, it handles the error and returns -1.
    ///
    uint32_t getUniformIndex(const std::string &uniformName);

    static constexpr uint32_t maxSize{32};
    std::array<uint32_t, maxSize> indices;
    core::Pipeline *pipeline;
    std::unordered_map<std::string, uint32_t> uniformMap; ///< Map to store buffer names and their indices

    uint32_t currentUniformCount{0};
};

} // namespace vkf::scene

#endif // VULKANRENDERER_MATERIALCOMPONENT_H