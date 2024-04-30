/// \file
/// \brief

//
// Created by Joshua Lowe on 1/22/2024.
// The license and distribution terms for this file may be found in the file LICENSE in this distribution
//

#include "BoundingBoxComponent.h"
#include "../../common/GeometryHandling.h"
#include <imgui.h>

namespace vkf::scene
{

std::vector<float> BoundingBoxComponent::getMeshData()
{
    // this is x and z
    std::vector<float> mesh = {
        bbox[0], bbox[3], // llcrnrlon, urcrnrlat
        bbox[2], bbox[3], // urcrnrlon, urcrnrlat
        bbox[0], bbox[1], // llcrnrlon, llcrnrlat
        bbox[2], bbox[1], // urcrnrlon, llcrnrlat
    };
    return mesh;
}

Met3D::RectF BoundingBoxComponent::getRectF()
{
    return Met3D::RectF(bbox[0], bbox[1], bbox[2], bbox[3]);
}

void BoundingBoxComponent::updateGui()
{
    ImGui::Text("Bounding Box:");
    ImGui::Spacing();

    if (isInput)
    {
        if (ImGui::InputFloat4("##Bbox", bbox.data()))
        {
            hasNewBbox = true;
        }
    }
    else
    {
        if (ImGui::DragFloat4("##Bbox", bbox.data()))
        {
            hasNewBbox = true;
        }
    }

    ImGui::Spacing();

    if (ImGui::Button("Reset Global"))
    {
        bbox = {-180., -90., 180., 90.};
        hasNewBbox = true;
    }

    ImGui::Spacing();
}

} // namespace vkf::scene