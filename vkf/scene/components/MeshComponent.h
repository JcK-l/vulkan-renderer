////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file MeshComponent.h
/// \brief This file declares the MeshComponent struct which is used for managing mesh data for an entity in a scene.
///
/// The MeshComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage mesh
/// data.
///
/// \author Joshua Lowe
/// \date 1/3/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_MESHCOMPONENT_H
#define VULKANRENDERER_MESHCOMPONENT_H

#include "../../core/Buffer.h"

namespace vkf::scene
{

///
/// \struct MeshComponent
/// \brief Struct for managing mesh data for an entity in a scene.
///
/// This struct provides functionality to store and manage mesh data. It contains a shared pointer to a Buffer object to
/// store the mesh data, a uint32_t to store the number of vertices and a boolean to determine if the mesh should be
/// drawn.
///
struct MeshComponent
{
    ///
    /// \brief Constructor that takes a Device object and a vector of floats representing the mesh as parameters.
    ///
    /// This constructor initializes the vertexBuffer member with a Buffer object created with the provided Device
    /// object and mesh data. It also initializes the numVertices member with the size of the provided mesh data.
    ///
    explicit MeshComponent(const core::Device &device, std::vector<float> mesh);

    void displayGui();

    std::shared_ptr<core::Buffer> vertexBuffer;
    uint32_t numVertices;
    bool shouldDraw = true;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_MESHCOMPONENT_H