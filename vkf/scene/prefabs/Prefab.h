////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Prefab.h
/// \brief This file declares the abstract Prefab class which is used for managing prefabricated entities in a scene.
///
/// The Prefab class is part of the vkf::scene namespace.
/// It provides an abstract interface for creating and managing prefabricated entities.
///
/// \author Joshua Lowe
/// \date 1/6/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../Entity.h"

// Forward declarations
#include "../../common/CommonFwd.h"
#include "../../core/CoreFwd.h"
#include "../../rendering/RenderingFwd.h"

namespace vkf::scene
{

// Forward declarations
class Camera;
class Scene;

///
/// \class Prefab
/// \brief Abstract class for managing prefabricated entities in a scene.
///
/// This abstract class provides an interface for creating and managing prefabricated entities.
/// It contains pure virtual methods that must be implemented by any concrete subclass.
///
class Prefab
{
  public:
    virtual ~Prefab() = default; ///< Virtual destructor

    virtual UUID create(const core::Device &device, std::deque<core::Pipeline *> pipelines, Scene *scene,
                        std::string tag) = 0;

    virtual void destroy() = 0;

    virtual entt::entity getEntity();

    ///
    /// \brief Pure virtual method to display GUI for a prefab.
    ///
    /// This method displays the GUI for a prefab. It must be implemented in any concrete subclass.
    ///
    virtual void updateGui() = 0;

    ///
    /// \brief Pure virtual method to update components of a prefab.
    ///
    /// This method updates the components of a prefab. It must be implemented in any concrete subclass.
    ///
    virtual void updateComponents() = 0;

    ///
    /// \brief Static method to get a PipelineBuilder for a prefab.
    ///
    /// This method takes a Device object, a RenderPass object and a BindlessManager object as parameters and returns a
    /// PipelineBuilder for a prefab.
    ///
    static rendering::PipelineBuilder getPipelineBuilder(const core::Device &device, const core::RenderPass &renderPass,
                                                         rendering::BindlessManager &bindlessManager);

  protected:
    ///
    /// \brief Constructor that takes a registry, a BindlessManager object and an Entity object as parameters.
    ///
    /// This constructor initializes the registry, bindlessManager and entity members with the provided parameters.
    ///
    explicit Prefab(entt::registry &registry, rendering::BindlessManager &bindlessManager, Entity entity)
        : registry{registry}, bindlessManager{bindlessManager}, entity{std::move(entity)}
    {
    }

    entt::registry &registry;
    rendering::BindlessManager &bindlessManager;
    Entity entity;
};

} // namespace vkf::scene