////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file BasemapActor.h
/// \brief This file declares the BasemapActor class, which is a type of Prefab in the vkf::scene namespace.
///
/// The BasemapActor class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// BasemapActor prefab. A BasemapActor prefab in this context is a specific type of Prefab that can be used in a 3D
/// scene.
///
/// \author Joshua Lowe
/// \date 1/9/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Prefab.h"
#include "PrefabTypeManager.h"
#include <entt/entt.hpp>

namespace vkf::scene
{

///
/// \class BasemapActor
/// \brief This class manages BasemapActor prefabs.
///
/// This class provides an interface for creating and managing a BasemapActor prefab. A BasemapActor prefab in this
/// context is a specific type of Prefab that can be used in a 3D scene.
///
class BasemapActor : public Prefab
{
  public:
    ///
    /// \brief Constructs a BasemapActor object.
    ///
    /// This constructor initializes the registry reference with the provided registry, by passing it to the Base class.
    ///
    /// \param registry Reference to the entt::registry
    /// \param bindlessManager Reference to the BindlessManager
    /// \param entity The entity to use for creating the BasemapActor entity.
    ///
    explicit BasemapActor(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity);

    ///
    /// \brief Creates a BasemapActor prefab.
    ///
    /// This method creates a BasemapActor prefab using the provided device, bindlessManager, pipeline, camera, and tag.
    ///
    /// \param device The Vulkan device to use for creating the Texture2D entity.
    /// \param bindlessManager The BindlessManager to use for creating the Texture2D entity.
    /// \param pipelines The Pipelines to use for creating the Texture2D entity.
    /// \param camera The Camera to use for creating the Texture2D entity.
    /// \param tag The tag to use for creating the Texture2D entity.
    ///
    UUID create(const core::Device &device, std::deque<core::Pipeline *> pipelines, Scene *scene,
                std::string tag) override;

    void destroy() override;

    ///
    /// \brief Displays the GUI for the BasemapActor prefab.
    ///
    /// This method is overridden from the Prefab base class and is used to display the GUI for the BasemapActor prefab.
    ///
    void updateGui() override;

    ///
    /// \brief Updates the components of the BasemapActor prefab.
    ///
    /// This method is overridden from the Prefab base class and is used to update the components of the BasemapActor
    /// prefab.
    ///
    /// \param bindlessManager The BindlessManager to use for updating the components of the BasemapActor prefab.
    ///
    void updateComponents() override;

    static uint32_t vertexSize;

    static std::deque<rendering::PipelineBuilder> getPipelineBuilders(const core::Device &device,
                                                                      const core::RenderPass &renderPass,
                                                                      rendering::BindlessManager &bindlessManager);
};

} // namespace vkf::scene