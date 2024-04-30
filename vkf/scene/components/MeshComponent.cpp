////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file MeshComponent.cpp
/// \brief This file implements the MeshComponent struct which is used for managing mesh data for an entity in a scene.
///
/// The MeshComponent struct is part of the vkf::scene namespace. It provides functionality to store and manage mesh
/// data.
///
/// \author Joshua Lowe
/// \date 1/3/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MeshComponent.h"
#include "../../core/Device.h"
#include <imgui.h>

namespace vkf::scene
{

MeshComponent::MeshComponent(const core::Device &device) : device{device}
{
}

void MeshComponent::updateGui()
{
    ImGui::Text("Mesh:");
    ImGui::Spacing();
    std::string checkboxLabel = "Drawable##" + std::to_string(reinterpret_cast<uintptr_t>(this));
    ImGui::Checkbox(checkboxLabel.c_str(), &shouldDraw);
    ImGui::Spacing();
}

void MeshComponent::uploadGeometry(std::vector<float> mesh, uint32_t vertexSize)
{
    device.getHandle().waitIdle();
    vertexBuffer = std::make_shared<core::Buffer>(
        device,
        vk::BufferCreateInfo{.size = sizeof(float) * mesh.size(),
                             .usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst},
        VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);

    vk::BufferCreateInfo bufferCreateInfo{.size = sizeof(float) * mesh.size(),
                                          .usage = vk::BufferUsageFlagBits::eTransferSrc};
    core::Buffer stagingBuffer{device, bufferCreateInfo,
                               VMA_ALLOCATION_CREATE_MAPPED_BIT |
                                   VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT};

    stagingBuffer.updateData(mesh.data(), sizeof(float) * mesh.size(), 0);

    vertexBuffer->copyBuffer(stagingBuffer);
    numVertices = static_cast<uint32_t>(mesh.size()) / (vertexSize / sizeof(float));
}

} // namespace vkf::scene