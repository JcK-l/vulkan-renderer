////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Texture2D.h
/// \brief This file declares the Texture2D class, which is a type of Prefab in the vkf::scene namespace.
///
/// The Texture2D class is part of the vkf::scene namespace. It provides an interface for creating and managing a
/// Texture2D prefab. A Texture2D prefab in this context is a specific type of Prefab that can be used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
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
/// \class Texture2D
/// \brief This class manages Texture2D prefabs.
///
/// This class provides an interface for creating and managing a Texture2D prefab. A Texture2D entity in this context is
/// a specific type of Prefab that can be used in a 3D scene.
///
class Texture2D : public Prefab
{
  public:
    ///
    /// \brief Constructs a Texture2D object.
    ///
    /// This constructor initializes the registry reference with the provided registry, by passing it to the Base class.
    ///
    /// \param registry Reference to the entt::registry
    /// \param bindlessManager Reference to the BindlessManager
    /// \param entity The entity to use for creating the Texture2D entity.
    ///
    explicit Texture2D(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity);

    ///
    /// \brief Creates a Texture2D prefab.
    ///
    /// This method creates a Texture2D prefab using the provided device, bindlessManager, pipeline, camera, and tag.
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
    /// \brief Displays the GUI for the Texture2D prefab.
    ///
    /// This method is overridden from the Prefab base class and is used to display the GUI for the Texture2D prefab.
    ///
    void updateGui() override;

    ///
    /// \brief Updates the components of the Texture2D prefab.
    ///
    /// This method is overridden from the Prefab base class and is used to update the components of the Texture2D
    /// prefab.
    ///
    /// \param bindlessManager The BindlessManager to use for updating the components of the Texture2D prefab.
    ///
    void updateComponents() override;

    static uint32_t vertexSize;

    static std::deque<rendering::PipelineBuilder> getPipelineBuilders(const core::Device &device,
                                                                      const core::RenderPass &renderPass,
                                                                      rendering::BindlessManager &bindlessManager);
};

} // namespace vkf::scene