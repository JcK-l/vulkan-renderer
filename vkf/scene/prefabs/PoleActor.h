////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file PoleActor.h
/// \brief This file declares the PoleActor class, which is a type of Prefab in the vkf::scene namespace.
///
/// The PoleActor class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// PoleActor Prefab. A PoleActor Prefab in this context is a specific type of Prefab that can be used in a 3D
/// scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Prefab.h"
#include <glm/vec4.hpp>

namespace vkf::scene
{

// Forward declarations
struct MeshComponent;

enum class PoleType
{
    Line,
    Tick
};

///
/// \class PoleActor
/// \brief This class manages PoleActor prefabs.
///
/// This class provides an interface for creating and managing a GraticuleActor prefab. A GraticuleActor prefab in this
/// context is a specific type of Prefab that can be used in a 3D scene.
///
class PoleActor : public Prefab
{
  public:
    explicit PoleActor(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity);

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
    void createPole();
    void destroyPole();
    void uploadGeometry(PoleType type, MeshComponent &meshComponent);

    const core::Device *device{};
    std::deque<core::Pipeline *> pipelines;
    Scene *scene;

    glm::vec4 prevColor;
};

} // namespace vkf::scene