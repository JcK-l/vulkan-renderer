////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Cube.h
/// \brief This file declares the Cube class, which is a type of Entity in the vkf::scene namespace.
///
/// The Cube class is part of the vkf::scene namespace. It provides an interface for creating and managing a Cube
/// entity. A Cube entity in this context is a specific type of Entity that can be used in a 3D scene.
///
/// \author Joshua Lowe
/// \date 1/2/2024
///
/// The license and distribution terms for this file may be found in the file LICENSE in this distribution
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef VULKANRENDERER_CUBE_H
#define VULKANRENDERER_CUBE_H

#include "../Entity.h"

namespace vkf::core // Forward declarations
{
class Device;
class Pipeline;
} // namespace vkf::core

namespace vkf::scene
{

// Forward declarations
class Camera;

///
/// \class Cube
/// \brief This class manages Cube entities.
///
/// This class provides an interface for creating and managing a Cube entity. A Cube entity in this context is a
/// specific type of Entity that can be used in a 3D scene.
///
class Cube : public Entity
{
  public:
    ///
    /// \brief Constructs a Cube object.
    ///
    /// This constructor initializes the registry reference with the provided registry, by passing it to the Base class.
    ///
    /// \param registry Reference to the entt::registry
    ///
    explicit Cube(entt::registry &registry, rendering::BindlessManager &bindlessManager)
        : Entity(registry, bindlessManager){};

    ///
    /// \brief Creates a Cube entity.
    ///
    /// This method creates a Cube entity using the provided device, bindlessManager, pipeline, camera, and tag.
    ///
    /// \param device The Vulkan device to use for creating the Cube entity.
    /// \param bindlessManager The BindlessManager to use for creating the Cube entity.
    /// \param pipeline The Pipeline to use for creating the Cube entity.
    /// \param camera The Camera to use for creating the Cube entity.
    /// \param tag The tag to use for creating the Cube entity.
    ///
    void create(const core::Device &device, core::Pipeline *pipeline, Camera *camera, std::string tag);

    void destroy() override;

    ///
    /// \brief Displays the GUI for the Cube entity.
    ///
    /// This method is overridden from the Entity base class and is used to display the GUI for the Cube entity.
    ///
    void displayGui() override;

    ///
    /// \brief Updates the components of the Cube entity.
    ///
    /// This method is overridden from the Entity base class and is used to update the components of the Cube entity.
    ///
    /// \param bindlessManager The BindlessManager to use for updating the components of the Cube entity.
    ///
    void updateComponents() override;
};

} // namespace vkf::scene

#endif // VULKANRENDERER_CUBE_H