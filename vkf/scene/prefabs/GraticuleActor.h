////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file GraticuleActor.h
/// \brief This file declares the GraticuleActor class, which is a type of Prefab in the vkf::scene namespace.
///
/// The GraticuleActor class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// GraticuleActor Prefab. A GraticuleActor Prefab in this context is a specific type of Prefab that can be used in a 3D
/// scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../common/GeometryHandling.h"
#include "Prefab.h"
#include <glm/vec4.hpp>

namespace vkf::scene
{

// Forward declarations
struct MeshComponent;

enum class GraticuleType
{
    Graticule,
    Coastline,
    Borderline
};

///
/// \class GraticuleActor
/// \brief This class manages GraticuleActor prefabs.
///
/// This class provides an interface for creating and managing a GraticuleActor prefab. A GraticuleActor prefab in this
/// context is a specific type of Prefab that can be used in a 3D scene.
///
class GraticuleActor : public Prefab
{
  public:
    explicit GraticuleActor(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity);

    UUID create(const core::Device &device, std::deque<core::Pipeline *> pipelines, Scene *scene,
                std::string tag) override;

    void destroy() override;

    void updateGui() override;

    void updateComponents() override;

    static uint32_t vertexSize;

    static std::deque<rendering::PipelineBuilder> getPipelineBuilders(const core::Device &device,
                                                                      const core::RenderPass &renderPass,
                                                                      rendering::BindlessManager &bindlessManager);

  private:
    void uploadGeometry(GraticuleType type, MeshComponent &meshComponent);

    std::vector<std::vector<Met3D::PointF>> projectAndClipGeometry(Met3D::GeometryHandling *geo,
                                                                   std::vector<std::vector<Met3D::PointF>> geometry,
                                                                   Met3D::RectF bbox,
                                                                   double rotatedGridMaxSegmentLength_deg);

    Met3D::RectF bbox = {-180., -90., 180., 90.};
    glm::vec4 prevColor;
    uint32_t entityBufferModelHandle;
};

} // namespace vkf::scene